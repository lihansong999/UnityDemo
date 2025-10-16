using System.Collections;
using System.Collections.Generic;
using Unity.VisualScripting;
using UnityEngine;


public class NetworkManager : MonoBehaviour
{
    static public NetworkManager instance { get; private set; }
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

    private NetworkClient _client;

    public bool isConnected { get; private set; } = false;
    public bool isLogined { get; private set; } = false;
    public bool isGameing { get; private set; } = false;
    private const float HeartbeatResetTime = 12f;
    private float _nextHeartbeatSubTime = HeartbeatResetTime;

    private void Start()
    {
        _client = new NetworkClient("127.0.0.1", 15551, "ws://127.0.0.1:15552");
        _client.OnConnected += OnNetConnected;
        _client.OnConnectFailed += OnNetConnectFailed;
        _client.OnDisconnected += OnNetDisconnected;
        _client.OnMessage += OnNetMessage;
    }

    private void Update()
    {
        TimedHeartbeat();
    }

    private void TimedHeartbeat()
    {
#if !UNITY_WEBGL
        if (isConnected && isLogined)
        {
            _nextHeartbeatSubTime -= Time.deltaTime;
            if (_nextHeartbeatSubTime <= 0)
            {
                _nextHeartbeatSubTime = HeartbeatResetTime;
                MsgCSHeartbeat req;
                req.head.type = (byte)GameMsgType.MSG_TYPE_HEARTBEAT;
                _ = _client.Send<MsgCSHeartbeat>(req);
            }
        }
#endif
    }

    private void OnApplicationQuit()
    {
        _client.Dispose();
    }

    private void OnNetConnected()
    {
        isConnected = true;

        // ���ӳɹ���ֱ�������¼
        MsgCSLogin req;
        req.head.type = (byte)GameMsgType.MSG_TYPE_LOGIN;
        _ = _client.Send<MsgCSLogin>(req);
    }

    private void OnNetConnectFailed()
    {
        UIManager.instance.ShowConnectFailed();
    }

    private void OnNetDisconnected()
    {
        isConnected = false;
        isLogined = false;
        isGameing = false;

        if (TimeSyncManager.instance != null) 
        {
            TimeSyncManager.instance.SwitchAutoSync(false);
        }

        if (GameManager.instance != null)
        {
            GameManager.instance.InitGame();
        }

        if (UIManager.instance != null)
        {
            UIManager.instance.SwitchGameingUI(false);
            UIManager.instance.SwitchSettingUI(false);
            UIManager.instance.ShowDisconnected();
        }
    }

    private void OnNetMessage(byte[] data)
    {
        MsgHead head = NetWorkUtils.BytesToStruct<MsgHead>(data);
        switch (head.type)
        {
            case (byte)GameMsgType.MSG_TYPE_HEARTBEAT: // ����
                {                
                    // var hb = NetWorkUtils.BytesToStruct<MsgSCHeartbeat>(data);
                    // Debug.Log("�յ�����");
                }
                break;
            case (byte)GameMsgType.MSG_TYPE_LOGIN: // ��¼
                {
                    var login = NetWorkUtils.BytesToStruct<MsgSCLogin>(data);
                    isLogined = true;
                    GameManager.instance.selfEntityID = login.selfEntityID;
                    // ��¼�ɹ���ֱ��ͬ��һ��ʱ��
                    TimeSyncManager.instance.RequestServerSync();
                }
                break;
            case (byte)GameMsgType.MSG_TYPE_LOGOUT: // �ǳ�
                {
                    var logout = NetWorkUtils.BytesToStruct<MsgSCLogout>(data);
                    GameManager.instance.RemovePlayerFromGame(logout.outEntityID);
                }
                break;
            case (byte)GameMsgType.MSG_TYPE_SYNCTIME: // ͬ��ʱ��
                {
                    var syncTime = NetWorkUtils.BytesToStruct<MsgSCSyncTime>(data);
                    TimeSyncManager.instance.OnSyncServerTime(syncTime.serverTime, syncTime.reqTime);
                    if (!isGameing)
                    {
                        TimeSyncManager.instance.SwitchAutoSync(true);

                        // ��һ��ͬ����ʱ������������Ϸ
                        MsgCSAddGame req;
                        req.head.type = (byte)GameMsgType.MSG_TYPE_SELFADDGAME;
                        _ = _client.Send<MsgCSAddGame>(req);
                    }
                }
                break;
            case (byte)GameMsgType.MSG_TYPE_SELFADDGAME: // �Լ�������Ϸ
                {
                    isGameing = true;
                    var addGame = NetWorkUtils.BytesToStruct<MsgSCAddGame>(data);
                    GameManager.instance.AddSelfPlayerToGameForBirth(addGame.birthPoint);
                    UIManager.instance.SwitchNetWorkUI(false);
                    UIManager.instance.SwitchGameingUI(true);
                }
                break;
            case (byte)GameMsgType.MSG_TYPE_PLAYER_FRAME: // ֡
                {
                    var frame = NetWorkUtils.BytesToStruct<MsgSCFrame>(data);
                    GameManager.instance.FrameComing(frame.frameEntityID, frame.frameInfo);
                }
                break;
            case (byte)GameMsgType.MSG_TYPE_FLY_EF_FRAME: // ֡
                {
                    var frame = NetWorkUtils.BytesToStruct<MsgSCFlyEffectFrame>(data);
                    GameManager.instance.MagicFrameComing(frame);
                }
                break;
            default:
                Debug.LogWarning($"δ֪��Ϣ����: {head.type}");
                break;
        }
    }

    // ���ӷ�����
    public void ConnectServer()
    {
        _ = _client.Connect();
    }

    // �ǳ�������
    public void LogoutServer()
    {
        if (isConnected)
        {
            if (isLogined)
            {
                MsgCSLogout req;
                req.head.type = (byte)GameMsgType.MSG_TYPE_LOGOUT;
                _ = _client.Send<MsgCSLogout>(req);
            }
            else
            {
                _client.Dispose();
            }
        }
    }

    // ����ͬ��ʱ��
    public void RequestSyncTime(long clientSendTime)
    {
        if (isConnected && isLogined)
        {
            _nextHeartbeatSubTime = HeartbeatResetTime;
            MsgCSSyncTime req;
            req.head.type = (byte)GameMsgType.MSG_TYPE_SYNCTIME;
            req.reqTime = clientSendTime;
            _ = _client.Send<MsgCSSyncTime>(req);
        }
    }

    // ����֡��Ϣ
    public void SendFrame(in NetRoleFrame frame)
    {
        if (isConnected && isLogined && isGameing)
        {
            _nextHeartbeatSubTime = HeartbeatResetTime;
            MsgCSFrame req;
            req.head.type = (byte)GameMsgType.MSG_TYPE_PLAYER_FRAME;
            req.frameInfo = frame;
            _ = _client.Send<MsgCSFrame>(req);
        }
    }

    // ����ʩ����Ϣ
    public void SendLaunchMagic(ref MsgCSFlyEffectFrame req)
    {
        if (isConnected && isLogined && isGameing)
        {
            _nextHeartbeatSubTime = HeartbeatResetTime;
            req.head.type = (byte)GameMsgType.MSG_TYPE_FLY_EF_FRAME;
            _ = _client.Send<MsgCSFlyEffectFrame>(req);
        }
    }
}
