#ifndef __FRAMEWORK_WSGOSVCMANAGEBASE_H__
#define __FRAMEWORK_WSGOSVCMANAGEBASE_H__

#include "FWWsGoSvcTypeDef.h"

FW_NS_BEGIN


/*
WebSocket服务管理基类
可接受连接，以及管理连接的套接字
继承后使用
*/
class FW_DLL FWWsGoSvcManageBase
{
	friend unsigned char _wss_onWsSvcReject(void* self, long long sid, const char* host);
	friend void _wss_onWsSvcEstablishe(void* self, long long sid, long long cid);
	friend void _wss_onWsSvcMessage(void* self, long long sid, long long cid, char* newBuf, int size);
	friend void _wss_onWsSvcClose(void* self, long long sid, long long cid);
public:
	enum WsGoDataProtocol
	{
		WSGODP_TEXT = 0,
		WSGODP_BINARY,
	};
public:
	FWWsGoSvcManageBase();
	virtual ~FWWsGoSvcManageBase();

	/*
	port：监听端口号
	pattern：路径，即url的路径部分（以'/'开头，没有可传NULL）
	subprotocol：子协议，即JavaScript客户端WebSocket(url, protocols)的第2个参数（没有可传NULL）
	maxConnectedCount：最大承载连接数
	dataProtocol：协议数据类型
	maxMessageSize：接收单个message的最大长度，超过这个长度的会被视为非法连接，立即断开
	rsize：读取缓冲区大小（建议设置为大于每一个经常接收的消息）
	wsize：写入缓冲区大小（建议设置为大于每一个经常发送的消息）
	sendChannelSize：golang的发送Channel缓冲区大小（建议设置为大于同一时刻并行发送的个数）
	handshakeTimeout：websocket握手超时时间
	writeWait：写入发送缓冲区超时时间
	pongWait：pong的等待时间，超过等待时间没有收到pong会视为连接已断开
	pingPeriod：ping的发送间隔时间
	*/
	bool Start(unsigned short port,
		const char * pattern,
		const char * subprotocol,
		int maxConnectedCount,
		WsGoDataProtocol dataProtocol,
		int maxMessageSize,
		int rsize = FW_WSGOSVC_RECV_BUF_SIZE,
		int wsize = FW_WSGOSVC_SEND_BUF_SIZE,
		int sendChannelSize = FW_WSGOSVC_SEND_CHAN_SIZE,
		int handshakeTimeout = 10,
		int writeWait = 10,
		int pongWait = 60,
		int pingPeriod = 50);

	/*
	certFilePath：公钥文件路径
	keyFilePath：私钥文件路径
	*/
	bool StartTLS(unsigned short port,
		const char* pattern,
		const char* subprotocol,
		const char* certFilePath,
		const char* keyFilePath,
		int maxConnectedCount,
		WsGoDataProtocol dataProtocol,
		int maxMessageSize,
		int rsize = FW_WSGOSVC_RECV_BUF_SIZE,
		int wsize = FW_WSGOSVC_SEND_BUF_SIZE,
		int sendChannelSize = FW_WSGOSVC_SEND_CHAN_SIZE,
		int handshakeTimeout = 10,
		int writeWait = 10,
		int pongWait = 60,
		int pingPeriod = 50);

	void Stop();

	bool SendDataWsGoSvcSocket(const FWNetMatch& netMatch, const char* pData, int size);

	void SendDataWsGoSvcSocketBatch(const FWNetMatch* netMatches, unsigned int count, const char* pData, int size);

	void CloseWsGoSvcSocket(const FWNetMatch& netMatch);

	void CloseWsGoSvcSocketBatch(const FWNetMatch* netMatches, unsigned int count);

	bool GetWsGoSvcSocketInfo(const FWNetMatch& netMatch, FWSvcSocketInfo* out_svcSocketInfo);

	int GetConnectedCount();

	/*
	用于释放在golang模块中申请的c内存
	例如释放_onWsGoSvcMessage回调中的newBuf参数
	*/
	static void GoFree(void * pointer);

	/*
	获取golang模块中的时间
	*/
	static long long GoTime();

	/*
	设置golang线程的最大数量，同时返回之前的设置，默认为CPU核心数
	如果传小于0的值，则不改变设置
	*/
	static int GoMaxprocs(int maxProcs);

protected:
	/*
	是否拒绝此次握手协议，默认始终返回false
	true：拒绝
	false：不拒绝
	*/
	virtual bool _onWsGoSvcReject(const char* host);

	/*
	如果连接数已达上线，此时有连接进来会被自动断开，并调用此函数
	*/
	virtual void _onWsGoSvcOverloaded();

	/*
	握手协议已完成
	*/
	virtual void _onWsGoSvcEstablishe(const FWNetMatch& netMatch) = 0;

	/*
	接收已完成
	newBuf：message数据缓冲区，使用完成后必须调用GoFree释放（包括size为0时，只要newBuf!=NULL就必须调用GoFree释放）
	size：newBuf中的数据大小（支持0大小的message）
	*/
	virtual void _onWsGoSvcMessage(const FWNetMatch& netMatch, char* newBuf, unsigned int size) = 0;

	/*
	连接已断开
	*/
	virtual void _onWsGoSvcClose(const FWNetMatch& netMatch) = 0;

private:
	volatile bool	_running;

	long long		_sid;

	long			_maxConnectedCount;

	volatile long	_curConnectedCount;

	void*			_wss_callback;
};


FW_NS_END


#endif //!__FRAMEWORK_WSGOSVCMANAGEBASE_H__