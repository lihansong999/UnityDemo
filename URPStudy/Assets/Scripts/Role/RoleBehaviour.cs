using System.Collections;
using System.Collections.Generic;
using Unity.VisualScripting;
using UnityEngine;

public enum RoleMoveState
{
    Idle = 0,
    Walk,
    Run,
}

public class RoleBehaviour : EntityBehaviour
{
    public GameObject _firstNode { get; private set; }
    public GameObject _thirdNode { get; private set; }

    private bool _mountedCamera = false;

    private Vector3 _thirdNodeOriginPos;
    private Vector3 _thirdNodeDownPos; // �����˳����Ͽ�ʱ��һ��������ڵ���λ��
    
    protected float _curYRotation { get; private set; } = 0f;   // ��ǰY�����ת�Ƕ�
    private bool _yRotationChange = false;

    protected float _curXRotation { get; private set; } = 0f;   // ��ǰX�����ת�Ƕ�

    private Animator _animator; // �ӽڵ��ɫģ�͵� Animator

    protected virtual void Awake()
    {
        _firstNode = transform.Find("FirstNode").gameObject;
        _thirdNode = transform.Find("ThirdNode").gameObject;

        _thirdNodeOriginPos = _thirdNode.transform.localPosition;
        _thirdNodeDownPos.Set(_thirdNodeOriginPos.x, _thirdNodeOriginPos.y - 4.7f, _thirdNodeOriginPos.z);

        _animator = GetComponentInChildren<Animator>(true);
    }

    protected virtual void Start()
    {
        
    }

    protected virtual void OnDestroy()
    {
        UnMountCameraObject();
    }

    protected virtual void Update()
    {

    }

    protected virtual void FixedUpdate()
    {

    }

    protected virtual void LateUpdate()
    {
        if (_yRotationChange)
        {
            transform.localRotation = Quaternion.Euler(0f, _curYRotation, 0f);
        }
        if (_mountedCamera)
        {
            _firstNode.transform.localRotation = Quaternion.Euler(_curXRotation, 0f, 0f);
            _thirdNode.transform.localRotation = Quaternion.Euler(_curXRotation + 11f, 0f, 0f);
            if (GameCameraManager.instance.curGameCameraMode == GameCameraMode.Third)
            {
                float d = GameCameraManager.instance.thirdXRotationClamp.y - GameCameraManager.instance.thirdXRotationClamp.x;
                float c = GameCameraManager.instance.thirdXRotationClamp.y - _curXRotation;
                float r = c / d;
                _thirdNode.transform.localPosition = _thirdNodeOriginPos + (_thirdNodeDownPos - _thirdNodeOriginPos) * r;
            }
        }
    }

    // �����������ɫ
    public void MountCameraObject(GameObject first, GameObject third)
    {
        if(!_mountedCamera)
        {
            _mountedCamera = true;

            first.transform.SetParent(_firstNode.transform, false);
            third.transform.SetParent(_thirdNode.transform, false);
        }
    }

    // ж�����
    public void UnMountCameraObject()
    {
        if(_mountedCamera)
        {
            _mountedCamera = false;

            var fc = _firstNode.GetComponentInChildren<Camera>(true);
            if (fc != null)
                fc.gameObject.transform.SetParent(null, false);

            var tc = _thirdNode.GetComponentInChildren<Camera>(true);
            if (tc != null)
                tc.gameObject.transform.SetParent(null, false);
        }
    }

    // ��Y����ת��ת��
    public void AddYRotation(float r)
    {
        if (r != 0f)
        {
            _yRotationChange = true;
            _curYRotation += r;
        }
    }

    // ����Y����ת��ת��
    public void SetYRotation(float r)
    {
        _yRotationChange = true;
        _curYRotation = r;
    }

    // ��X����ת��̧ͷ��
    public void AddXRotation(float r)
    {
        _curXRotation += r;
    }

    // ����X����ת��̧ͷ��
    public void SetXRotation(float r)
    {
        _curXRotation = r;
    }

    public bool AnimIsPlayAttack()
    {
        return _animator.IsNameForPlaying(0, "Attack", true);
    }

    public bool AnimIsPlayJump()
    {
        return _animator.IsNameForPlaying(0, "jump", true);
    }

    public bool AnimIsPlayBeHit()
    {
        return _animator.IsNameForPlaying(0, "behit", true);
    }

    public bool AnimIsPlayDie()
    {
        return _animator.IsNameForPlaying(0, "die", true);
    }

    public RoleMoveState AnimGetMoveState()
    {
        return (RoleMoveState)_animator.GetInteger("moveState");
    }

    public void AnimSetMoveState(RoleMoveState s)
    {
        _animator.SetInteger("moveState", (int)s);
    }

    public void AnimPlayJump()
    {
        _animator.SetTrigger("jump");
    }

    public void AnimPlayAttack()
    {
        _animator.SetTrigger("attack");
    }

    public void AnimPlayBeHit()
    {
        _animator.SetTrigger("behit");
    }

    public void AnimPlayDie()
    {
        _animator.SetTrigger("die");
    }

}
