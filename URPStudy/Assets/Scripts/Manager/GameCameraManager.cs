using System.Collections;
using System.Collections.Generic;
using Unity.VisualScripting;
using UnityEngine;
using UnityEngine.SceneManagement;

public enum GameCameraMode
{
    FreeFly,
    First,
    Third,
}

public class GameCameraManager : MonoBehaviour
{
    static public GameCameraManager instance { get; private set; }
    private void Awake()
    {
        if (instance != null && instance != this)
        {
            Debug.LogError("repeat create manage object name: " + this.GetType());
            Destroy(gameObject); // ����������Ĺ������
            return;
        }
        instance = this;
        // DontDestroyOnLoad(gameObject); // �糡������
        _cameraFirst = Instantiate(_cameraFirstPrefab);
        _cameraThird = Instantiate(_cameraThirdPrefab);
    }
    private void OnDestroy()
    {
        if (this == instance)
        {
            instance = null;
        }
    }

    [SerializeField] private GameObject _cameraFreeFly;
    [SerializeField] private GameObject _cameraFirstPrefab;
    [SerializeField] private GameObject _cameraThirdPrefab;
    private GameObject _cameraFirst;
    private GameObject _cameraThird;
    private GameObject _trackTarget; // ��һ�˳ƺ͵����˳�������ٵ�Ŀ��
    public Vector2 firstXRotationClamp { get; private set; } = new Vector2(-80f, 50f);
    public Vector2 thirdXRotationClamp { get; private set; } = new Vector2(-80f, 20f);

    public GameCameraMode curGameCameraMode { get; private set; } = GameCameraMode.FreeFly;

    private void Start()
    {

    }

    private void Update()
    {
        HandleOperation();
    }

    private void HandleOperation()
    {
        if (GameManager.instance.curGameStatus == GameStatus.Fighting)
        {
            if (!GameManager.instance.IsShowingUIForGameOperation())
            {
                if (Input.GetKeyUp(KeyCode.V))
                {
                    if (curGameCameraMode == GameCameraMode.Third)
                    {
                        SetCurCameraMode(GameCameraMode.First);
                    }
                    else if (curGameCameraMode == GameCameraMode.First)
                    {
                        SetCurCameraMode(GameCameraMode.Third);
                    }
                }
            }
        }
        else if (GameManager.instance.curGameStatus == GameStatus.Watching)
        {

        }
    }

    // ���ã���Ϸ��ʼ��ʱ�ã�
    public void ResetCamera()
    {
        if(_trackTarget != null)
        {
            _trackTarget.GetComponent<RoleBehaviour>().UnMountCameraObject();
            _trackTarget = null;
        }
        SetCurCameraMode(GameCameraMode.FreeFly);
    }

    // ���õ�ǰ���ģʽ
    public void SetCurCameraMode(GameCameraMode m)
    {
        if (curGameCameraMode != m)
        {
            var origin = curGameCameraMode;
            curGameCameraMode = m;

            if (origin == GameCameraMode.FreeFly)
                _cameraFreeFly.SetActive(false);
            else if (origin == GameCameraMode.First)
                _cameraFirst.SetActive(false);
            else if (origin == GameCameraMode.Third)
                _cameraThird.SetActive(false);

            if (curGameCameraMode == GameCameraMode.FreeFly)
                _cameraFreeFly.SetActive(true);
            else if (curGameCameraMode == GameCameraMode.First)
                _cameraFirst.SetActive(true);
            else if (curGameCameraMode == GameCameraMode.Third)
                _cameraThird.SetActive(true);
        }
    }

    // ���õ�һ�͵����˳��������Ŀ��
    public void SetCamera13TrackTarget(GameObject trackTarget)
    {
        if (_trackTarget != trackTarget)
        {
            if (_trackTarget != null)
            {
                _trackTarget.GetComponent<RoleBehaviour>().UnMountCameraObject();
                _trackTarget = null;
            }
            _trackTarget = trackTarget;
            if (_trackTarget != null)
            {
                _trackTarget.GetComponent<RoleBehaviour>().MountCameraObject(_cameraFirst, _cameraThird);
            }
        }
    }

    // �ƶ����������ĳ��Ŀ���Ϸ���������
    public void MoveCameraFTrackTarget(GameObject t)
    {
        _cameraFreeFly.GetComponent<CameraFreeFlyBehaviour>().ToTargetUp(t);
    }
}
