using UnityEngine;

public class TimeSyncManager : MonoBehaviour
{
    public static TimeSyncManager instance { get; private set; }

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

    // ----------------------- 内部变量 -----------------------

    public long _serverTimeAtSync { get; private set; }       // 上次同步时的服务器时间 (毫秒)
    private long _localTimeAtSync = 0;        // 上次同步时的本地时间 (毫秒)

    private bool _autoSync = false;         // 定期自动同步开关
    private long _lastSyncTime = 0;         // 重同步计时器
    private const long _syncInterval = 30000;   // 默认30秒同步一次

    private long _lastEffectiveSyncTime = 0; // 上次生效的同步时间
    public long _lastRTT { get; private set; } = 99999999; // 上次RTT

    // ----------------------- 外部接口 -----------------------

    /// <summary>
    /// 获取当前服务器时间（毫秒）
    /// </summary>
    public long GetServerTime()
    {
        long now = (long)(Time.realtimeSinceStartup * 1000);
        return _serverTimeAtSync + (now - _localTimeAtSync);
    }

    /// <summary>
    /// 开关自动同步
    /// </summary>
    public void SwitchAutoSync(bool autoSync)
    {
        if(!_autoSync && autoSync)
            _lastSyncTime = (long)(Time.realtimeSinceStartup * 1000);
        _autoSync = autoSync;
    }

    /// <summary>
    /// 向服务器请求一次时间同步
    /// </summary>
    public void RequestServerSync()
    {
        _lastSyncTime = (long)(Time.realtimeSinceStartup * 1000);
        NetworkManager.instance.RequestSyncTime((long)(Time.realtimeSinceStartup * 1000));
    }

    /// <summary>
    /// 当服务器返回时间时调用
    /// </summary>
    /// <param name="serverTime">服务器时间（毫秒）</param>
    /// <param name="clientSendTime">客户端发起请求时的本地时间（毫秒）</param>
    public void OnSyncServerTime(long serverTime, long clientSendTime)
    {
        long now = (long)(Time.realtimeSinceStartup * 1000);
        long rtt = now - clientSendTime;

        // 根据不同rtt决定下一次自动同步时间
        if (rtt > 500)
            _lastSyncTime = now - (long)(_syncInterval * 0.84f);
        else if (rtt > 200)
            _lastSyncTime = now - (long)(_syncInterval * 0.67f);
        else if (rtt > 100)
            _lastSyncTime = now - (long)(_syncInterval * 0.5f);
        else
            _lastSyncTime = now;

        // 根据rtt和上一次同步生效时间来决定这次是否作为有效同步
        if (rtt > _lastRTT) {
            long subEffective = now - _lastEffectiveSyncTime;
            if (rtt > 500) {
                if (subEffective < 3600000)
                    return;
            }
            else if (rtt > 260) {
                if (subEffective < 180000)
                    return;
            }
            else if (rtt > 100) {
                if (subEffective < 60000)
                    return;
            }
        }

        _lastEffectiveSyncTime = now;
        _lastRTT = rtt;

        // 估算服务器当前时间（加上一定RTT延迟）
        long estimatedServerTime = serverTime + (long)(rtt / 2.2f);
        long currentLocalServerTime = _serverTimeAtSync + (now - _localTimeAtSync);
        long diff = estimatedServerTime - currentLocalServerTime;

        if (Mathf.Abs(diff) > 150) {
            // 误差太大，直接对齐
            _serverTimeAtSync = estimatedServerTime;
            _localTimeAtSync = now;
        }
        else {
            // 平滑修正
            _serverTimeAtSync += diff / 2;
        }
    }

    // ----------------------- 内部逻辑 -----------------------

    private void Update()
    {
        if (_autoSync)
        {
            long now = (long)(Time.realtimeSinceStartup * 1000);
            if (now - _lastSyncTime > _syncInterval)
            {
                _lastSyncTime = now;
                RequestServerSync();
            }
        }
    }
}
