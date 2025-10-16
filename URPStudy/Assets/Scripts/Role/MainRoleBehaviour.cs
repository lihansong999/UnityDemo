using System;
using System.Collections;
using System.Collections.Generic;
using Unity.Mathematics;
using Unity.VisualScripting;
using UnityEngine;

public class MainRoleBehaviour : RoleBehaviour
{
    private CharacterController _controller;
    private float _verticalVelocity;
    private Vector3 _horizontalVelocity = new Vector3();
    private float _baseStepOffset;

    private float _operateXRotation;

    private bool _isJumping = false;

    private NetRoleFrame lastSendFrameInfo = new NetRoleFrame();    // 上一次发送的帧数据
    private float _lastSendFrameTime = 0f;        // 上一次发送帧数据的时间
    private float _nextFrameIntervalTime = 0f;    // 下一次发送间隔时间
    private bool _lastMouseIsOperation = false;   // 上一帧鼠标是否有移动视角操作
    private Vector3 _beforeCallMovePosition = new Vector3(); // 调用move之前的位置

    IEnumerator DelayInvoke(float delay, Action callback)
    {
        yield return new WaitForSeconds(delay);
        callback?.Invoke();
    }

    protected override void Start()
    {
        base.Start();
        _controller = GetComponent<CharacterController>();
        _controller.enabled = true; // 预制体禁用，在脚本启用，避免初次位置设置失效
        _baseStepOffset = _controller.stepOffset;
    }

    protected override void Update()
    {
        base.Update();

        if (GameCameraManager.instance.curGameCameraMode != GameCameraMode.FreeFly) // 只有死了才会切换到自由相机
        {
            var syncFrameType = FrameInfoType.PosRotate;
            bool last2MouseIsOperation = _lastMouseIsOperation;
            var lastAnimMoveState = AnimGetMoveState();
            bool lastIsGrounded = _controller.isGrounded;

            _lastMouseIsOperation = false;

            bool attacking = AnimIsPlayAttack();
            bool suiShowing = GameManager.instance.IsShowingUIForGameOperation();
            if (!suiShowing && !attacking)
            {
                float mx = Input.GetAxis("Mouse X");
                if (mx != 0f)
                {
                    AddYRotation(mx * GlobalValueManager.instance.mouseSensitivity * Time.deltaTime);
                }
                float my = Input.GetAxis("Mouse Y");
                if (my != 0f)
                {
                    if (GameCameraManager.instance.curGameCameraMode == GameCameraMode.First)
                    {
                        _operateXRotation -= my * GlobalValueManager.instance.mouseSensitivity * Time.deltaTime;
                        _operateXRotation = Mathf.Clamp(_operateXRotation, GameCameraManager.instance.firstXRotationClamp.x, GameCameraManager.instance.firstXRotationClamp.y);
                        SetXRotation(_operateXRotation);
                    }
                    else if (GameCameraManager.instance.curGameCameraMode == GameCameraMode.Third)
                    {
                        _operateXRotation -= my * GlobalValueManager.instance.mouseSensitivity * Time.deltaTime;
                        _operateXRotation = Mathf.Clamp(_operateXRotation, GameCameraManager.instance.thirdXRotationClamp.x, GameCameraManager.instance.thirdXRotationClamp.y);
                        SetXRotation(_operateXRotation);
                    }
                }
                _lastMouseIsOperation = (mx != 0f || my != 0f);
            }

            _verticalVelocity += GlobalValueManager.WORLD_GRAVITY * Time.deltaTime;

            if (_controller.isGrounded)
            {
                _horizontalVelocity.Set(0, 0, 0);

                _controller.stepOffset = _baseStepOffset;

                _verticalVelocity = -2f; // 贴地就初始化轻微下压

                if (!suiShowing && !attacking)
                {
                    // 平面移动
                    float h = Input.GetAxis("Horizontal");
                    float v = Input.GetAxis("Vertical");
                    if (h != 0f || v != 0f)
                    {
                        _horizontalVelocity = (transform.right * h + transform.forward * v);
                        if (Input.GetKey(KeyCode.LeftShift))
                        {
                            _horizontalVelocity *= GlobalValueManager.ROLE_RUN_SPEED;
                            AnimSetMoveState(RoleMoveState.Run);
                        }
                        else
                        {
                            _horizontalVelocity *= GlobalValueManager.ROLE_WALK_SPEED;
                            AnimSetMoveState(RoleMoveState.Walk);
                        }
                    }
                    else
                    {
                        AnimSetMoveState(RoleMoveState.Idle);
                    }

                    if (Input.GetButtonDown("Jump"))
                    {
                        _verticalVelocity = Mathf.Sqrt(GlobalValueManager.ROLE_JUMP_HEIGHT * -2f * GlobalValueManager.WORLD_GRAVITY);

                        _controller.stepOffset = 0.0f;

                        _isJumping = true;

                        AnimPlayJump();
                        syncFrameType = FrameInfoType.JumpStart;
                    }
                    else if (Input.GetMouseButtonDown(0))
                    {
                        AnimPlayAttack();
                        syncFrameType = FrameInfoType.AttackStart;

                        // 发射飞行特效 攻击动画是大约在0.35的时候出手
                        StartCoroutine(DelayInvoke(0.35f, () =>
                        {
                            LaunchMagic();
                        }));
                        //long wt = 350 - TimeSyncManager.instance._lastRTT;
                        //if (wt > 15)
                        //{
                        //    StartCoroutine(DelayInvoke(wt /1000f, () =>
                        //    {
                        //        LaunchMagic();
                        //    }));
                        //}
                        //else
                        //{
                        //    LaunchMagic();
                        //}
                    }
                }
                else
                {
                    AnimSetMoveState(RoleMoveState.Idle);
                }
            }
            else
            {
                // 可以考虑在这里先提前处理一下MoveState，但要注意出生时的自由下落情况要排除
            }

            _beforeCallMovePosition = transform.position;

            _controller.Move(new Vector3(_horizontalVelocity.x, _verticalVelocity, _horizontalVelocity.z) * Time.deltaTime);

            if (_isJumping && _controller.isGrounded /*&& !AnimIsPlayJump()*/)
                _isJumping = false;

            var moveState = AnimGetMoveState();

            if (lastAnimMoveState != moveState
                || syncFrameType == FrameInfoType.JumpStart
                || syncFrameType == FrameInfoType.AttackStart)
            {
                // 移动或攻击跳跃等状态变化时立即同步
                _nextFrameIntervalTime = 0f;
            }
            else if (last2MouseIsOperation != _lastMouseIsOperation
                || lastIsGrounded != _controller.isGrounded)
            {
                // 鼠标状态变化、触地状态变化均采用加速同步
                _nextFrameIntervalTime = math.min(_nextFrameIntervalTime, GlobalValueManager.MAX_CONSECUTIVE_FRAME_INTERVAL / 2f);
            }
            else if (moveState != RoleMoveState.Idle
                || _lastMouseIsOperation
                || !_controller.isGrounded)
            {
                // 非静止时快速同步
                _nextFrameIntervalTime = math.min(_nextFrameIntervalTime, GlobalValueManager.MAX_CONSECUTIVE_FRAME_INTERVAL);
            }

            if (_lastSendFrameTime != 0 && _nextFrameIntervalTime > GlobalValueManager.MAX_CONSECUTIVE_FRAME_INTERVAL && lastSendFrameInfo.curTime > 0)
            {
                Vector3 pos = transform.position;
                if (lastSendFrameInfo.positionX != NetWorkUtils.f2i(pos.x) || lastSendFrameInfo.positionY != NetWorkUtils.f2i(pos.y) || lastSendFrameInfo.positionZ != NetWorkUtils.f2i(pos.z))
                {
                    // 被动移动时快速同步
                    _nextFrameIntervalTime = math.min(_nextFrameIntervalTime, GlobalValueManager.MAX_CONSECUTIVE_FRAME_INTERVAL);
                }
            }

            if (_nextFrameIntervalTime < 1f / 60f || Time.realtimeSinceStartup - _lastSendFrameTime >= _nextFrameIntervalTime)
                SyncFrame(syncFrameType);
        }
    }

    private void SyncFrame(FrameInfoType ft)
    {
        //AirBox.Show("发送 " + ft + "  " + Time.realtimeSinceStartup);

        _nextFrameIntervalTime = 5f; // 每次发送后都假定立刻静止，静止时5秒才同步一次
       _lastSendFrameTime = Time.realtimeSinceStartup;
        lastSendFrameInfo.curTime = TimeSyncManager.instance.GetServerTime();
        lastSendFrameInfo.infoType = (byte)ft;
        lastSendFrameInfo.animMoveState = (byte)AnimGetMoveState();
        Vector3 pos = transform.position;
        if (ft == FrameInfoType.JumpStart)
            pos = _beforeCallMovePosition; // 起跳帧用起跳前一刻的位置
        lastSendFrameInfo.positionX = NetWorkUtils.f2i(pos.x);
        lastSendFrameInfo.positionY = NetWorkUtils.f2i(pos.y);
        lastSendFrameInfo.positionZ = NetWorkUtils.f2i(pos.z);
        lastSendFrameInfo.rotationX = NetWorkUtils.f2i(_curXRotation);
        lastSendFrameInfo.rotationY = NetWorkUtils.f2i(_curYRotation);
        lastSendFrameInfo.horizontalVelocityX = NetWorkUtils.f2i(_horizontalVelocity.x);
        lastSendFrameInfo.horizontalVelocityZ = NetWorkUtils.f2i(_horizontalVelocity.z);
        lastSendFrameInfo.isJumping = (byte)(_isJumping ? 1 : 0);
        //if (UnityEngine.Random.Range(0, 4) == 0)
        //    return; // 模拟大量丢包
        NetworkManager.instance.SendFrame(lastSendFrameInfo);
    }

    private void LaunchMagic()
    {
        const float FlyEffectSpeed = 100.0f; // 飞行特效速度（每秒）

        Vector3 pos = transform.position;
        Quaternion rotation = Quaternion.Euler(_curXRotation, _curYRotation, 0);

        Vector3 velocity = new Vector3(0, GameCameraManager.instance.curGameCameraMode == GameCameraMode.Third ? -17 : 1, 100); // 暂时手动调整发射方向
        velocity *= FlyEffectSpeed / velocity.magnitude;
        velocity = rotation * velocity;

        MsgCSFlyEffectFrame req = new MsgCSFlyEffectFrame();
        req.curTime = TimeSyncManager.instance.GetServerTime();
        req.positionX = NetWorkUtils.f2i(pos.x);
        req.positionY = NetWorkUtils.f2i(pos.y + 0.2f);
        req.positionZ = NetWorkUtils.f2i(pos.z);
        req.velocityX = NetWorkUtils.f2i(velocity.x);
        req.velocityY = NetWorkUtils.f2i(velocity.y);
        req.velocityZ = NetWorkUtils.f2i(velocity.z);

        NetworkManager.instance.SendLaunchMagic(ref req);
    }
}
