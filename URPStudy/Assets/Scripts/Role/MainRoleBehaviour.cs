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

    private NetRoleFrame lastSendFrameInfo = new NetRoleFrame();    // ��һ�η��͵�֡����
    private float _lastSendFrameTime = 0f;        // ��һ�η���֡���ݵ�ʱ��
    private float _nextFrameIntervalTime = 0f;    // ��һ�η��ͼ��ʱ��
    private bool _lastMouseIsOperation = false;   // ��һ֡����Ƿ����ƶ��ӽǲ���
    private Vector3 _beforeCallMovePosition = new Vector3(); // ����move֮ǰ��λ��

    IEnumerator DelayInvoke(float delay, Action callback)
    {
        yield return new WaitForSeconds(delay);
        callback?.Invoke();
    }

    protected override void Start()
    {
        base.Start();
        _controller = GetComponent<CharacterController>();
        _controller.enabled = true; // Ԥ������ã��ڽű����ã��������λ������ʧЧ
        _baseStepOffset = _controller.stepOffset;
    }

    protected override void Update()
    {
        base.Update();

        if (GameCameraManager.instance.curGameCameraMode != GameCameraMode.FreeFly) // ֻ�����˲Ż��л����������
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

                _verticalVelocity = -2f; // ���ؾͳ�ʼ����΢��ѹ

                if (!suiShowing && !attacking)
                {
                    // ƽ���ƶ�
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

                        // ���������Ч ���������Ǵ�Լ��0.35��ʱ�����
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
                // ���Կ�������������ǰ����һ��MoveState����Ҫע�����ʱ�������������Ҫ�ų�
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
                // �ƶ��򹥻���Ծ��״̬�仯ʱ����ͬ��
                _nextFrameIntervalTime = 0f;
            }
            else if (last2MouseIsOperation != _lastMouseIsOperation
                || lastIsGrounded != _controller.isGrounded)
            {
                // ���״̬�仯������״̬�仯�����ü���ͬ��
                _nextFrameIntervalTime = math.min(_nextFrameIntervalTime, GlobalValueManager.MAX_CONSECUTIVE_FRAME_INTERVAL / 2f);
            }
            else if (moveState != RoleMoveState.Idle
                || _lastMouseIsOperation
                || !_controller.isGrounded)
            {
                // �Ǿ�ֹʱ����ͬ��
                _nextFrameIntervalTime = math.min(_nextFrameIntervalTime, GlobalValueManager.MAX_CONSECUTIVE_FRAME_INTERVAL);
            }

            if (_lastSendFrameTime != 0 && _nextFrameIntervalTime > GlobalValueManager.MAX_CONSECUTIVE_FRAME_INTERVAL && lastSendFrameInfo.curTime > 0)
            {
                Vector3 pos = transform.position;
                if (lastSendFrameInfo.positionX != NetWorkUtils.f2i(pos.x) || lastSendFrameInfo.positionY != NetWorkUtils.f2i(pos.y) || lastSendFrameInfo.positionZ != NetWorkUtils.f2i(pos.z))
                {
                    // �����ƶ�ʱ����ͬ��
                    _nextFrameIntervalTime = math.min(_nextFrameIntervalTime, GlobalValueManager.MAX_CONSECUTIVE_FRAME_INTERVAL);
                }
            }

            if (_nextFrameIntervalTime < 1f / 60f || Time.realtimeSinceStartup - _lastSendFrameTime >= _nextFrameIntervalTime)
                SyncFrame(syncFrameType);
        }
    }

    private void SyncFrame(FrameInfoType ft)
    {
        //AirBox.Show("���� " + ft + "  " + Time.realtimeSinceStartup);

        _nextFrameIntervalTime = 5f; // ÿ�η��ͺ󶼼ٶ����̾�ֹ����ֹʱ5���ͬ��һ��
       _lastSendFrameTime = Time.realtimeSinceStartup;
        lastSendFrameInfo.curTime = TimeSyncManager.instance.GetServerTime();
        lastSendFrameInfo.infoType = (byte)ft;
        lastSendFrameInfo.animMoveState = (byte)AnimGetMoveState();
        Vector3 pos = transform.position;
        if (ft == FrameInfoType.JumpStart)
            pos = _beforeCallMovePosition; // ����֡������ǰһ�̵�λ��
        lastSendFrameInfo.positionX = NetWorkUtils.f2i(pos.x);
        lastSendFrameInfo.positionY = NetWorkUtils.f2i(pos.y);
        lastSendFrameInfo.positionZ = NetWorkUtils.f2i(pos.z);
        lastSendFrameInfo.rotationX = NetWorkUtils.f2i(_curXRotation);
        lastSendFrameInfo.rotationY = NetWorkUtils.f2i(_curYRotation);
        lastSendFrameInfo.horizontalVelocityX = NetWorkUtils.f2i(_horizontalVelocity.x);
        lastSendFrameInfo.horizontalVelocityZ = NetWorkUtils.f2i(_horizontalVelocity.z);
        lastSendFrameInfo.isJumping = (byte)(_isJumping ? 1 : 0);
        //if (UnityEngine.Random.Range(0, 4) == 0)
        //    return; // ģ���������
        NetworkManager.instance.SendFrame(lastSendFrameInfo);
    }

    private void LaunchMagic()
    {
        const float FlyEffectSpeed = 100.0f; // ������Ч�ٶȣ�ÿ�룩

        Vector3 pos = transform.position;
        Quaternion rotation = Quaternion.Euler(_curXRotation, _curYRotation, 0);

        Vector3 velocity = new Vector3(0, GameCameraManager.instance.curGameCameraMode == GameCameraMode.Third ? -17 : 1, 100); // ��ʱ�ֶ��������䷽��
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
