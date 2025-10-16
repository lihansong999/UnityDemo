using UnityEngine;

public class TimeSyncManager : MonoBehaviour
{
    public static TimeSyncManager instance { get; private set; }

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

    // ----------------------- �ڲ����� -----------------------

    public long _serverTimeAtSync { get; private set; }       // �ϴ�ͬ��ʱ�ķ�����ʱ�� (����)
    private long _localTimeAtSync = 0;        // �ϴ�ͬ��ʱ�ı���ʱ�� (����)

    private bool _autoSync = false;         // �����Զ�ͬ������
    private long _lastSyncTime = 0;         // ��ͬ����ʱ��
    private const long _syncInterval = 30000;   // Ĭ��30��ͬ��һ��

    private long _lastEffectiveSyncTime = 0; // �ϴ���Ч��ͬ��ʱ��
    public long _lastRTT { get; private set; } = 99999999; // �ϴ�RTT

    // ----------------------- �ⲿ�ӿ� -----------------------

    /// <summary>
    /// ��ȡ��ǰ������ʱ�䣨���룩
    /// </summary>
    public long GetServerTime()
    {
        long now = (long)(Time.realtimeSinceStartup * 1000);
        return _serverTimeAtSync + (now - _localTimeAtSync);
    }

    /// <summary>
    /// �����Զ�ͬ��
    /// </summary>
    public void SwitchAutoSync(bool autoSync)
    {
        if(!_autoSync && autoSync)
            _lastSyncTime = (long)(Time.realtimeSinceStartup * 1000);
        _autoSync = autoSync;
    }

    /// <summary>
    /// �����������һ��ʱ��ͬ��
    /// </summary>
    public void RequestServerSync()
    {
        _lastSyncTime = (long)(Time.realtimeSinceStartup * 1000);
        NetworkManager.instance.RequestSyncTime((long)(Time.realtimeSinceStartup * 1000));
    }

    /// <summary>
    /// ������������ʱ��ʱ����
    /// </summary>
    /// <param name="serverTime">������ʱ�䣨���룩</param>
    /// <param name="clientSendTime">�ͻ��˷�������ʱ�ı���ʱ�䣨���룩</param>
    public void OnSyncServerTime(long serverTime, long clientSendTime)
    {
        long now = (long)(Time.realtimeSinceStartup * 1000);
        long rtt = now - clientSendTime;

        // ���ݲ�ͬrtt������һ���Զ�ͬ��ʱ��
        if (rtt > 500)
            _lastSyncTime = now - (long)(_syncInterval * 0.84f);
        else if (rtt > 200)
            _lastSyncTime = now - (long)(_syncInterval * 0.67f);
        else if (rtt > 100)
            _lastSyncTime = now - (long)(_syncInterval * 0.5f);
        else
            _lastSyncTime = now;

        // ����rtt����һ��ͬ����Чʱ������������Ƿ���Ϊ��Чͬ��
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

        // �����������ǰʱ�䣨����һ��RTT�ӳ٣�
        long estimatedServerTime = serverTime + (long)(rtt / 2.2f);
        long currentLocalServerTime = _serverTimeAtSync + (now - _localTimeAtSync);
        long diff = estimatedServerTime - currentLocalServerTime;

        if (Mathf.Abs(diff) > 150) {
            // ���̫��ֱ�Ӷ���
            _serverTimeAtSync = estimatedServerTime;
            _localTimeAtSync = now;
        }
        else {
            // ƽ������
            _serverTimeAtSync += diff / 2;
        }
    }

    // ----------------------- �ڲ��߼� -----------------------

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
