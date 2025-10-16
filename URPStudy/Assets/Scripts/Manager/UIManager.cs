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
            Destroy(gameObject); // 不创建多余的管理对象
            return;
        }
        instance = this;
        // DontDestroyOnLoad(gameObject); // 跨场景保留
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
        _lastSyncTime.SetText("同步时间：" + TimeSyncManager.instance._serverTimeAtSync);
        _curSvrTime.SetText("当前时间：" + TimeSyncManager.instance.GetServerTime());
        _lastSyncRTT.SetText("最后一次同步生效的RTT：" + TimeSyncManager.instance._lastRTT);
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

    //////////////////////////////////////////// 网络连接界面（启动开始游戏界面） ////////////////////////////////////////////

    [SerializeField] private GameObject _netWorkUI; // 网络界面
    [SerializeField] private TextMeshProUGUI _tips; // 文字提示
    [SerializeField] private GameObject _spinner; // 转菊花
    [SerializeField] private GameObject _btnStart; // 开始按钮

    private void InitNetWorkUI()
    {
        _netWorkUI.SetActive(true);
        _tips.SetText("点击按钮开始游戏");
        _spinner.SetActive(false);
        _btnStart.SetActive(true);
    }

    // 开关网络界面
    public void SwitchNetWorkUI(bool isShow)
    {
        _netWorkUI.SetActive(isShow);
    }

    // 网络界面是否显示中
    public bool IsShowingNetWorkUI()
    {
        return _netWorkUI.activeSelf;
    }

    // 显示连接中
    public void ShowConnecting()
    {
        _netWorkUI.SetActive(true);
        _tips.SetText("连接中...");
        _spinner.SetActive(true);
        _btnStart.SetActive(false);
    }

    // 显示连接失败
    public void ShowConnectFailed()
    {
        _netWorkUI.SetActive(true);
        _tips.SetText("连接失败，点击按钮重试");
        _spinner.SetActive(false);
        _btnStart.SetActive(true);
    }

    // 显示连接已断开
    public void ShowDisconnected()
    {
        _netWorkUI.SetActive(true);
        _tips.SetText("连接已断开，点击按钮重新开始游戏");
        _spinner.SetActive(false);
        _btnStart.SetActive(true);
    }

    // 点击连接按钮
    public void OnClickConnect()
    {
        ShowConnecting();
        NetworkManager.instance.ConnectServer();
    }

    // 点击登出按钮
    public void OnClickLogout()
    {
        NetworkManager.instance.LogoutServer();
    }

    //////////////////////////////////////////// 设置界面 ////////////////////////////////////////////

    [SerializeField] private GameObject _settingUI;

    // 开关设置界面
    public void SwitchSettingUI(bool isShow)
    {
        _settingUI.SetActive(isShow);
    }

    // 设置界面是否显示中
    public bool IsShowingSettingUI()
    {
        return _settingUI.activeSelf;
    }

    //////////////////////////////////////////// 游戏中界面 ////////////////////////////////////////////

    [SerializeField] private GameObject _gameingUI;
    [SerializeField] private TextMeshProUGUI _lastSyncTime;
    [SerializeField] private TextMeshProUGUI _curSvrTime;
    [SerializeField] private TextMeshProUGUI _lastSyncRTT;
    [SerializeField] private TextMeshProUGUI _FPS;
    private float _fpsTimeAccumulator = 0f;
    private int _fpsFrameCount = 0;

    // 初始化游戏中界面
    private void InitGameingUI()
    {
        _gameingUI.SetActive(false);
    }

    // 开关设置界面
    public void SwitchGameingUI(bool isShow)
    {
        _gameingUI.SetActive(isShow);
    }

    // 点击同步时间
    public void OnClickSyncTime()
    {
        TimeSyncManager.instance.RequestServerSync();
    }

}
