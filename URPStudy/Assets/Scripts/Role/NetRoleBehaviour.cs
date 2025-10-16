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

    // Ԥ�����
    //    Ԥ����Ծ���
    private bool _predict_Jumping = false;
    private float _predict_JumpStartPosY = 0f;  // ��������������Ҫ��Ϊ��Ŀ�����θ߶��ж�
    private Vector3 _predict_JumpVelocity = new Vector3();
    //    Ԥ���ƶ����
    private float _lastPosRotateFrameCompleteTime = 0f;  // ���һ���ƶ�֡���ʱ��

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

    // ���������ƶ�Ԥ��
    private void TryProcessPredictMove()
    {
        if (Time.realtimeSinceStartup - _lastPosRotateFrameCompleteTime < 0.055 // ���ֻԤ��3֡
            && !_predict_Jumping // Ԥ����Ծʱ�Ͳ�Ԥ���ƶ�
            && _lastNetRoleFrame.isJumping == 0 // ������Ծ�е�֡
            && (_lastNetRoleFrame.horizontalVelocityX != 0 || _lastNetRoleFrame.horizontalVelocityZ != 0) // ��ˮƽ�ٶ�ʱ��Ԥ��
            )
        {
            //AirBox.Show("Ԥ���ƶ�");
            transform.position = new Vector3(transform.position.x + NetWorkUtils.i2f(_lastNetRoleFrame.horizontalVelocityX) * Time.deltaTime, transform.position.y, transform.position.z + NetWorkUtils.i2f(_lastNetRoleFrame.horizontalVelocityZ) * Time.deltaTime);
        }
    }

    // ������ԾԤ��
    private void StartPredictJump(float jumpStartPosY, in Vector2 hVelocity)
    {
        _predict_Jumping = true;
        _controller.enabled = true;
        _predict_JumpStartPosY = jumpStartPosY;
        _predict_JumpVelocity.Set(hVelocity.x, Mathf.Sqrt(GlobalValueManager.ROLE_JUMP_HEIGHT * -2f * GlobalValueManager.WORLD_GRAVITY), hVelocity.y);
    }

    // ������ԾԤ��
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

    // ��ֵ�ṹ
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
    private FrameInfoType _curInterpAnimType; // ��ǰ��ֵ������Դ֡����
    private InterpAnimInfo _interpAnimInfo = new InterpAnimInfo(); // ��ǰִ�еĲ�ֵ����

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
                return 2; // ����ɵ���һ֡�������ƶ�Ԥ��
            }
            float cv = _interpAnimInfo.curTime / _interpAnimInfo.totalTime;
            if (!_interpAnimInfo.isJumping || _curInterpAnimType == FrameInfoType.JumpStart)
                transform.position = _interpAnimInfo.beginPos + (_interpAnimInfo.endPos - _interpAnimInfo.beginPos) * cv;
            var cr = _interpAnimInfo.beginRotate + (_interpAnimInfo.endRotate - _interpAnimInfo.beginRotate) * cv;
            SetXRotation(cr.x);
            SetYRotation(cr.y);
            return 1; // ��ֵ����û���֮ǰ�Ͳ�������һ֡
        }
        return 0;
    }

    private bool ProcessFrame()
    {
        bool processed = false;
        while (_netFrameQueue.Count > 0)
        {
            //AirBox.Show("���� " + (FrameInfoType)frameInfo.infoType + "  " + Time.realtimeSinceStartup);

            processed = true;

            bool wantContinue = false;
            NetRoleFrame frameInfo = _netFrameQueue.Dequeue();

            if (_lastNetRoleFrame.curTime > frameInfo.curTime)
            {
                Debug.LogWarning("_lastNetRoleFrame.curTime > frameInfo.curTime");
                continue; // ֡����
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
                else if (fsubt > GlobalValueManager.MAX_CONSECUTIVE_FRAME_INTERVAL * 2f) // ��������֡������������Ͽ���ȷ���ǴӾ�ֹ��������֡��û�б�Ҫ����ԭ��ֵ����
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

    // ͨ��֡��Ϣʱ����������ǰ��ֵ���ŵ�ʱ��ϵ��
    private float CalcAnimCoef(long frameInfoTime, long lastFrameInfoTime, FrameInfoType frameInfoType)
    {
        if (lastFrameInfoTime == 0)
            return 0f; // ���ڴ����һ֡
        long fsub = frameInfoTime - lastFrameInfoTime;
        if (fsub > (long)(GlobalValueManager.MAX_CONSECUTIVE_FRAME_INTERVAL * 2000f) && (frameInfoType == FrameInfoType.AttackStart || frameInfoType == FrameInfoType.JumpStart))
            return 0f; // ��������֡������������Ͽ���ȷ���ǴӾ�ֹ��������֡���Ӿ�ֹ����Ծ���Ӿ�ֹ�������������ֵ��
        if (fsub <= 16)
            return 0f; // ʱ��̫�������ֵ
        const long mixValue = 50;
        const long maxValue = 300; // �����İ�������������һ����
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
        //AirBox.Show("�յ� " + (FrameInfoType)frame.infoType + "  " + Time.realtimeSinceStartup);

        _netFrameQueue.Enqueue(frame);
        if (_netFrameQueue.Count > 25) // Ŀǰ��Ƶ�ÿ֡��������Ϊ�ؼ�֡��ֻ�������25֡������ѻ�����
            _netFrameQueue.Dequeue();
    }
}
