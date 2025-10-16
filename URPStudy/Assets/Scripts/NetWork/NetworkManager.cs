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

        // 连接成功后直接请求登录
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
            case (byte)GameMsgType.MSG_TYPE_HEARTBEAT: // 心跳
                {                
                    // var hb = NetWorkUtils.BytesToStruct<MsgSCHeartbeat>(data);
                    // Debug.Log("收到心跳");
                }
                break;
            case (byte)GameMsgType.MSG_TYPE_LOGIN: // 登录
                {
                    var login = NetWorkUtils.BytesToStruct<MsgSCLogin>(data);
                    isLogined = true;
                    GameManager.instance.selfEntityID = login.selfEntityID;
                    // 登录成功后直接同步一次时间
                    TimeSyncManager.instance.RequestServerSync();
                }
                break;
            case (byte)GameMsgType.MSG_TYPE_LOGOUT: // 登出
                {
                    var logout = NetWorkUtils.BytesToStruct<MsgSCLogout>(data);
                    GameManager.instance.RemovePlayerFromGame(logout.outEntityID);
                }
                break;
            case (byte)GameMsgType.MSG_TYPE_SYNCTIME: // 同步时间
                {
                    var syncTime = NetWorkUtils.BytesToStruct<MsgSCSyncTime>(data);
                    TimeSyncManager.instance.OnSyncServerTime(syncTime.serverTime, syncTime.reqTime);
                    if (!isGameing)
                    {
                        TimeSyncManager.instance.SwitchAutoSync(true);

                        // 第一次同步完时间后请求加入游戏
                        MsgCSAddGame req;
                        req.head.type = (byte)GameMsgType.MSG_TYPE_SELFADDGAME;
                        _ = _client.Send<MsgCSAddGame>(req);
                    }
                }
                break;
            case (byte)GameMsgType.MSG_TYPE_SELFADDGAME: // 自己加入游戏
                {
                    isGameing = true;
                    var addGame = NetWorkUtils.BytesToStruct<MsgSCAddGame>(data);
                    GameManager.instance.AddSelfPlayerToGameForBirth(addGame.birthPoint);
                    UIManager.instance.SwitchNetWorkUI(false);
                    UIManager.instance.SwitchGameingUI(true);
                }
                break;
            case (byte)GameMsgType.MSG_TYPE_PLAYER_FRAME: // 帧
                {
                    var frame = NetWorkUtils.BytesToStruct<MsgSCFrame>(data);
                    GameManager.instance.FrameComing(frame.frameEntityID, frame.frameInfo);
                }
                break;
            case (byte)GameMsgType.MSG_TYPE_FLY_EF_FRAME: // 帧
                {
                    var frame = NetWorkUtils.BytesToStruct<MsgSCFlyEffectFrame>(data);
                    GameManager.instance.MagicFrameComing(frame);
                }
                break;
            default:
                Debug.LogWarning($"未知消息类型: {head.type}");
                break;
        }
    }

    // 连接服务器
    public void ConnectServer()
    {
        _ = _client.Connect();
    }

    // 登出服务器
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

    // 请求同步时间
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

    // 发送帧消息
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

    // 发送施法消息
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
