using System.Collections;
using System.Collections.Generic;
using TMPro;
using UnityEngine;

public class UIManager : MonoBehaviour
{
    static public UIManager instance { get; private set; }
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
    }
    private void OnDestroy()
    {
        if (this == instance)
        {
            instance = null;
        }
    }

    private void Start()
    {
        InitGameingUI();
        InitNetWorkUI();
    }

    private void Update()
    {
        _lastSyncTime.SetText("ͬ��ʱ�䣺" + TimeSyncManager.instance._serverTimeAtSync);
        _curSvrTime.SetText("��ǰʱ�䣺" + TimeSyncManager.instance.GetServerTime());
        _lastSyncRTT.SetText("���һ��ͬ����Ч��RTT��" + TimeSyncManager.instance._lastRTT);
        _fpsTimeAccumulator += Time.unscaledDeltaTime;
        _fpsFrameCount++;
        if (_fpsTimeAccumulator >= 0.5f)
        {
            float fps = _fpsFrameCount / _fpsTimeAccumulator;
            _FPS.SetText(string.Format("{0:0.} FPS", fps));
            _fpsTimeAccumulator = 0f;
            _fpsFrameCount = 0;
        }
    }

    //////////////////////////////////////////// �������ӽ��棨������ʼ��Ϸ���棩 ////////////////////////////////////////////

    [SerializeField] private GameObject _netWorkUI; // �������
    [SerializeField] private TextMeshProUGUI _tips; // ������ʾ
    [SerializeField] private GameObject _spinner; // ת�ջ�
    [SerializeField] private GameObject _btnStart; // ��ʼ��ť

    private void InitNetWorkUI()
    {
        _netWorkUI.SetActive(true);
        _tips.SetText("�����ť��ʼ��Ϸ");
        _spinner.SetActive(false);
        _btnStart.SetActive(true);
    }

    // �����������
    public void SwitchNetWorkUI(bool isShow)
    {
        _netWorkUI.SetActive(isShow);
    }

    // ��������Ƿ���ʾ��
    public bool IsShowingNetWorkUI()
    {
        return _netWorkUI.activeSelf;
    }

    // ��ʾ������
    public void ShowConnecting()
    {
        _netWorkUI.SetActive(true);
        _tips.SetText("������...");
        _spinner.SetActive(true);
        _btnStart.SetActive(false);
    }

    // ��ʾ����ʧ��
    public void ShowConnectFailed()
    {
        _netWorkUI.SetActive(true);
        _tips.SetText("����ʧ�ܣ������ť����");
        _spinner.SetActive(false);
        _btnStart.SetActive(true);
    }

    // ��ʾ�����ѶϿ�
    public void ShowDisconnected()
    {
        _netWorkUI.SetActive(true);
        _tips.SetText("�����ѶϿ��������ť���¿�ʼ��Ϸ");
        _spinner.SetActive(false);
        _btnStart.SetActive(true);
    }

    // ������Ӱ�ť
    public void OnClickConnect()
    {
        ShowConnecting();
        NetworkManager.instance.ConnectServer();
    }

    // ����ǳ���ť
    public void OnClickLogout()
    {
        NetworkManager.instance.LogoutServer();
    }

    //////////////////////////////////////////// ���ý��� ////////////////////////////////////////////

    [SerializeField] private GameObject _settingUI;

    // �������ý���
    public void SwitchSettingUI(bool isShow)
    {
        _settingUI.SetActive(isShow);
    }

    // ���ý����Ƿ���ʾ��
    public bool IsShowingSettingUI()
    {
        return _settingUI.activeSelf;
    }

    //////////////////////////////////////////// ��Ϸ�н��� ////////////////////////////////////////////

    [SerializeField] private GameObject _gameingUI;
    [SerializeField] private TextMeshProUGUI _lastSyncTime;
    [SerializeField] private TextMeshProUGUI _curSvrTime;
    [SerializeField] private TextMeshProUGUI _lastSyncRTT;
    [SerializeField] private TextMeshProUGUI _FPS;
    private float _fpsTimeAccumulator = 0f;
    private int _fpsFrameCount = 0;

    // ��ʼ����Ϸ�н���
    private void InitGameingUI()
    {
        _gameingUI.SetActive(false);
    }

    // �������ý���
    public void SwitchGameingUI(bool isShow)
    {
        _gameingUI.SetActive(isShow);
    }

    // ���ͬ��ʱ��
    public void OnClickSyncTime()
    {
        TimeSyncManager.instance.RequestServerSync();
    }

}
