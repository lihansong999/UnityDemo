using System;
using System.Data;
using System.Net.Sockets;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
#if UNITY_WEBGL
using NativeWebSocket;
#endif

public class NetworkClient : IDisposable
{
    private string serverIP;
    private int tcpPort;
    private string wsUrl;

    private bool autoReconnect;
    private int reconnectDelay;

    private CancellationTokenSource cts;

#if UNITY_WEBGL
    private WebSocket websocket;
#else
    private TcpClient tcpClient;
    private NetworkStream stream;
    private Thread receiveThread;
#endif

    // 消息回调
    public event Action<byte[]> OnMessage;
    public event Action OnConnected;
    public event Action OnConnectFailed;
    public event Action OnDisconnected;


    // 保存 Unity 主线程的上下文
    private readonly SynchronizationContext unityContext;

    public NetworkClient(string ip, int tcpPort, string wsUrl, bool autoReconnect = true, int reconnectDelay = 3000)
    {
        this.serverIP = ip;
        this.tcpPort = tcpPort;
        this.wsUrl = wsUrl;
        this.autoReconnect = autoReconnect;
        this.reconnectDelay = reconnectDelay;

        // 记录当前线程的同步上下文（主线程）
        unityContext = SynchronizationContext.Current ?? new SynchronizationContext();
    }

    // ----------------- 连接 -----------------
    public async Task Connect()
    {
        cts = new CancellationTokenSource();

#if UNITY_WEBGL
        websocket = new WebSocket(wsUrl);
        bool disconnected = false;
        void HandleDisconnected()
        {
            if (disconnected) return;
            disconnected = true;
            PostToMainThread(() => OnDisconnected?.Invoke());
        }

        websocket.OnOpen += () =>
        {
            disconnected = false;
            PostToMainThread(() => OnConnected?.Invoke());
        };

        websocket.OnMessage += (bytes) =>
        {
            PostToMainThread(() => OnMessage?.Invoke(bytes));
        };

        websocket.OnError += (e) =>
        {
            HandleDisconnected();
            // 不触发 OnConnectFailed，这里只算后续断开
        };

        websocket.OnClose += (e) =>
        {
            HandleDisconnected();
        };

        try
        {
            await websocket.Connect();
        }
        catch (Exception ex)
        {
            PostToMainThread(() => OnConnectFailed?.Invoke());
        }

#else
        try
        {
            tcpClient = new TcpClient();
            await tcpClient.ConnectAsync(serverIP, tcpPort);

            stream = tcpClient.GetStream();
            PostToMainThread(() => OnConnected?.Invoke());

            // 启动后台线程接收
            receiveThread = new Thread(ReceiveLoop);
            receiveThread.IsBackground = true;
            receiveThread.Start();
        }
        catch (Exception)
        {
            PostToMainThread(() => OnConnectFailed?.Invoke());
        }
#endif
    }

    // ----------------- 接收循环 -----------------
#if !UNITY_WEBGL
    private void ReceiveLoop()
    {
        try
        {
            while (!cts.Token.IsCancellationRequested && tcpClient.Connected)
            {
                byte[] lengthBuffer = new byte[4];
                int read = stream.Read(lengthBuffer, 0, 4);
                if (read <= 0) break;

                int msgLength = BitConverter.ToInt32(lengthBuffer, 0);
                byte[] data = new byte[msgLength];

                int received = 0;
                while (received < msgLength)
                {
                    int chunk = stream.Read(data, received, msgLength - received);
                    if (chunk <= 0) break;
                    received += chunk;
                }

                PostToMainThread(() => OnMessage?.Invoke(data));
            }
        }
        catch
        {
            // 出错也算断开
        }
        finally
        {
            PostToMainThread(() => OnDisconnected?.Invoke());
        }
    }
#endif

    // ----------------- 发送 -----------------
    public async Task Send<T>(T obj) where T : struct
    {
        byte[] body = StructToBytes(obj);

#if UNITY_WEBGL
        if (websocket != null && websocket.State == WebSocketState.Open)
        {
            await websocket.Send(body);
        }
#else
        if (tcpClient != null && tcpClient.Connected)
        {
            int totalLength = body.Length;
            byte[] header = BitConverter.GetBytes(totalLength);

            byte[] packet = new byte[header.Length + body.Length];
            Buffer.BlockCopy(header, 0, packet, 0, header.Length);
            Buffer.BlockCopy(body, 0, packet, header.Length, body.Length);

            await stream.WriteAsync(packet, 0, packet.Length);
        }
#endif
    }

    // ----------------- 工具 -----------------
    private byte[] StructToBytes<T>(T str) where T : struct
    {
        int size = Marshal.SizeOf(str);
        byte[] arr = new byte[size];
        IntPtr ptr = Marshal.AllocHGlobal(size);
        Marshal.StructureToPtr(str, ptr, true);
        Marshal.Copy(ptr, arr, 0, size);
        Marshal.FreeHGlobal(ptr);
        return arr;
    }

    // 统一回到 Unity 主线程执行
    private void PostToMainThread(Action action)
    {
        unityContext.Post(_ => action?.Invoke(), null);
    }

    public void Dispose()
    {
        cts?.Cancel();

#if UNITY_WEBGL
        websocket?.Close();
#else
        stream?.Close();
        tcpClient?.Close();
        if (receiveThread != null && receiveThread.IsAlive)
        {
            receiveThread.Abort();
        }
#endif
    }
}
