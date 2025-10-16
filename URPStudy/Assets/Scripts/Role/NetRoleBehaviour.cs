using System;
using System.Collections;
using System.Collections.Generic;
using Unity.Burst.Intrinsics;
using Unity.Mathematics;
using Unity.VisualScripting;
using UnityEngine;

public class NetRoleBehaviour : RoleBehaviour
{
    private CharacterController _controller;

    private Queue<NetRoleFrame> _netFrameQueue = new Queue<NetRoleFrame>();
    private NetRoleFrame _lastNetRoleFrame = new NetRoleFrame();

    // 预测相关
    //    预测跳跃相关
    private bool _predict_Jumping = false;
    private float _predict_JumpStartPosY = 0f;  // 如果地形有起伏需要改为用目标点地形高度判断
    private Vector3 _predict_JumpVelocity = new Vector3();
    //    预测移动相关
    private float _lastPosRotateFrameCompleteTime = 0f;  // 最后一次移动帧完成时间

    protected override void Start()
    {
        base.Start();
        _controller = GetComponent<CharacterController>();
    }

    protected override void Update()
    {
        base.Update();

        ProcessPredictJump();

        int r = PlayInterpAnim();
        if (r != 1)
        {
            if (!ProcessFrame())
            {
                if (r != 2)
                    TryProcessPredictMove();
            }
        }
    }

    // 尝试启动移动预测
    private void TryProcessPredictMove()
    {
        if (Time.realtimeSinceStartup - _lastPosRotateFrameCompleteTime < 0.055 // 最多只预测3帧
            && !_predict_Jumping // 预测跳跃时就不预测移动
            && _lastNetRoleFrame.isJumping == 0 // 不是跳跃中的帧
            && (_lastNetRoleFrame.horizontalVelocityX != 0 || _lastNetRoleFrame.horizontalVelocityZ != 0) // 有水平速度时才预测
            )
        {
            //AirBox.Show("预测移动");
            transform.position = new Vector3(transform.position.x + NetWorkUtils.i2f(_lastNetRoleFrame.horizontalVelocityX) * Time.deltaTime, transform.position.y, transform.position.z + NetWorkUtils.i2f(_lastNetRoleFrame.horizontalVelocityZ) * Time.deltaTime);
        }
    }

    // 启动跳跃预测
    private void StartPredictJump(float jumpStartPosY, in Vector2 hVelocity)
    {
        _predict_Jumping = true;
        _controller.enabled = true;
        _predict_JumpStartPosY = jumpStartPosY;
        _predict_JumpVelocity.Set(hVelocity.x, Mathf.Sqrt(GlobalValueManager.ROLE_JUMP_HEIGHT * -2f * GlobalValueManager.WORLD_GRAVITY), hVelocity.y);
    }

    // 处理跳跃预测
    private void ProcessPredictJump()
    {
        if (!_predict_Jumping)
            return;
        Vector3 velocity = _predict_JumpVelocity;
        velocity.y += GlobalValueManager.WORLD_GRAVITY * Time.deltaTime;
        _controller.Move(velocity * Time.deltaTime);
        _predict_JumpVelocity = velocity;
        if (_controller.isGrounded)
        {
            _predict_Jumping = false;
            _controller.enabled = false;
        }
    }

    // 插值结构
    private struct InterpAnimInfo
    {
        public float totalTime;
        public float curTime;
        public Vector3 beginPos;
        public Vector3 endPos;
        public Vector2 beginRotate;
        public Vector2 endRotate;
        public byte endMoveState;
        public Vector2 hVelocity;
        public bool isJumping;
    }
    private FrameInfoType _curInterpAnimType; // 当前插值动画来源帧类型
    private InterpAnimInfo _interpAnimInfo = new InterpAnimInfo(); // 当前执行的插值动画

    private int PlayInterpAnim()
    {
        if (_curInterpAnimType != FrameInfoType.None)
        {
            _interpAnimInfo.curTime += Time.deltaTime;
            if (_interpAnimInfo.curTime >= _interpAnimInfo.totalTime)
            {
                if (!_interpAnimInfo.isJumping || _curInterpAnimType == FrameInfoType.JumpStart)
                    transform.position = _interpAnimInfo.endPos;
                SetXRotation(_interpAnimInfo.endRotate.x);
                SetYRotation(_interpAnimInfo.endRotate.y);
                AnimSetMoveState((RoleMoveState)_interpAnimInfo.endMoveState);
                switch (_curInterpAnimType)
                {
                    case FrameInfoType.PosRotate:
                        _lastPosRotateFrameCompleteTime = Time.realtimeSinceStartup;
                        break;
                    case FrameInfoType.JumpStart:
                        {
                            StartPredictJump(_interpAnimInfo.endPos.y, _interpAnimInfo.hVelocity);
                            AnimPlayJump();
                        }
                        break;
                    case FrameInfoType.AttackStart:
                        AnimPlayAttack();
                        break;
                    default:
                        break;
                }
                _curInterpAnimType = FrameInfoType.None;
                return 2; // 刚完成的这一帧，不做移动预测
            }
            float cv = _interpAnimInfo.curTime / _interpAnimInfo.totalTime;
            if (!_interpAnimInfo.isJumping || _curInterpAnimType == FrameInfoType.JumpStart)
                transform.position = _interpAnimInfo.beginPos + (_interpAnimInfo.endPos - _interpAnimInfo.beginPos) * cv;
            var cr = _interpAnimInfo.beginRotate + (_interpAnimInfo.endRotate - _interpAnimInfo.beginRotate) * cv;
            SetXRotation(cr.x);
            SetYRotation(cr.y);
            return 1; // 插值动画没完成之前就不计算下一帧
        }
        return 0;
    }

    private bool ProcessFrame()
    {
        bool processed = false;
        while (_netFrameQueue.Count > 0)
        {
            //AirBox.Show("处理 " + (FrameInfoType)frameInfo.infoType + "  " + Time.realtimeSinceStartup);

            processed = true;

            bool wantContinue = false;
            NetRoleFrame frameInfo = _netFrameQueue.Dequeue();

            if (_lastNetRoleFrame.curTime > frameInfo.curTime)
            {
                Debug.LogWarning("_lastNetRoleFrame.curTime > frameInfo.curTime");
                continue; // 帧乱序
            }

            Vector3 position = new Vector3(NetWorkUtils.i2f(frameInfo.positionX), NetWorkUtils.i2f(frameInfo.positionY), NetWorkUtils.i2f(frameInfo.positionZ));
            Vector2 horizontalVelocity = new Vector2(NetWorkUtils.i2f(frameInfo.horizontalVelocityX), NetWorkUtils.i2f(frameInfo.horizontalVelocityZ));
            float xRotate = NetWorkUtils.i2f(frameInfo.rotationX);
            float yRotate = NetWorkUtils.i2f(frameInfo.rotationY);
            float coef = CalcAnimCoef(frameInfo.curTime, _lastNetRoleFrame.curTime, (FrameInfoType)frameInfo.infoType);
            if (coef <= 0f)
            {
                if (frameInfo.isJumping == 0 || (FrameInfoType)frameInfo.infoType == FrameInfoType.JumpStart)
                    transform.position = position;
                SetXRotation(xRotate);
                SetYRotation(yRotate);
                AnimSetMoveState((RoleMoveState)frameInfo.animMoveState);
                switch ((FrameInfoType)frameInfo.infoType)
                {
                    case FrameInfoType.PosRotate:
                        _lastPosRotateFrameCompleteTime = Time.realtimeSinceStartup;
                        break;
                    case FrameInfoType.JumpStart:
                        {
                            StartPredictJump(position.y, horizontalVelocity);
                            AnimPlayJump();
                        }
                        break;
                    case FrameInfoType.AttackStart:
                        AnimPlayAttack();
                        break;
                    default:
                        break;
                }
                wantContinue = true;
            }
            else
            {
                float fsubt = (frameInfo.curTime - _lastNetRoleFrame.curTime) * 0.001f;
                if (fsubt < 0f)
                    fsubt = 0f;
                else if (fsubt > GlobalValueManager.MAX_CONSECUTIVE_FRAME_INTERVAL * 2f) // 超过连续帧间隔两倍基本上可以确定是从静止到触发的帧，没有必要按照原差值计算
                    fsubt = GlobalValueManager.MAX_CONSECUTIVE_FRAME_INTERVAL * 0.75f;
                _curInterpAnimType = (FrameInfoType)frameInfo.infoType;
                _interpAnimInfo.curTime = 0f;
                _interpAnimInfo.totalTime = fsubt * coef;
                _interpAnimInfo.beginPos = transform.position;
                _interpAnimInfo.endPos = position;
                _interpAnimInfo.beginRotate.Set(_curXRotation, _curYRotation);
                _interpAnimInfo.endRotate.Set(xRotate, yRotate);
                _interpAnimInfo.endMoveState = frameInfo.animMoveState;
                _interpAnimInfo.hVelocity = horizontalVelocity;
                _interpAnimInfo.isJumping = frameInfo.isJumping != 0;
            }

            _lastNetRoleFrame = frameInfo;
            if (!wantContinue)
                break;
        }
        return processed;
    }

    // 通过帧信息时间来决定当前插值播放的时间系数
    private float CalcAnimCoef(long frameInfoTime, long lastFrameInfoTime, FrameInfoType frameInfoType)
    {
        if (lastFrameInfoTime == 0)
            return 0f; // 正在处理第一帧
        long fsub = frameInfoTime - lastFrameInfoTime;
        if (fsub > (long)(GlobalValueManager.MAX_CONSECUTIVE_FRAME_INTERVAL * 2000f) && (frameInfoType == FrameInfoType.AttackStart || frameInfoType == FrameInfoType.JumpStart))
            return 0f; // 超过连续帧间隔两倍基本上可以确定是从静止到触发的帧，从静止到跳跃、从静止到攻击都无需差值。
        if (fsub <= 16)
            return 0f; // 时间太短无需插值
        const long mixValue = 50;
        const long maxValue = 300; // 美国的包都随便可以来回一次了
        const long diffValue = maxValue - mixValue;
        long sub = TimeSyncManager.instance.GetServerTime() - frameInfoTime;
        if (sub <= mixValue)
            return 1f;
        if (sub >= maxValue)
            return 0f;
        sub -= mixValue;
        float f = sub / (float)diffValue;
        return 1f - f * f;
    }

    public void AddNetFrame(in NetRoleFrame frame)
    {
        //AirBox.Show("收到 " + (FrameInfoType)frame.infoType + "  " + Time.realtimeSinceStartup);

        _netFrameQueue.Enqueue(frame);
        if (_netFrameQueue.Count > 25) // 目前设计的每帧都可以作为关键帧，只保留最后25帧，避免堆积过多
            _netFrameQueue.Dequeue();
    }
}
