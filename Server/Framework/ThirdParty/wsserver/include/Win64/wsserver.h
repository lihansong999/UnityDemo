#ifndef __WSSERVER_H__
#define __WSSERVER_H__

#ifdef __cplusplus
extern "C" {
#endif

	/*
	连接信息
	*/
	typedef struct {
		char *host;
		long long initTime;
		long long lastReceivedTime;
	}wss_socket_info_t;

	/*
	回调结构
	*/
	typedef struct {
		void* self;
		void* onWsSvcReject;
		void* onWsSvcEstablishe;
		void* onWsSvcMessage;
		void* onWsSvcClose;
	}wss_callback_t;

	/*
	是否拒绝此次握手协议，默认始终返回0
	1：拒绝
	0：不拒绝
	*/
	typedef unsigned char (*wss_callback_onWsSvcReject)(void *self, long long sid, const char *host);
	/*
	握手协议已完成
	*/
	typedef void (*wss_callback_onWsSvcEstablishe)(void *self, long long sid, long long cid);
	/*
	接收已完成
	arg4：message数据缓冲区，使用完成后必须调用GO_FREE释放（包括arg5为0时，只要arg4!=NULL就必须调用GO_FREE释放）
	arg5：数据大小（支持0大小的message）
	*/
	typedef void (*wss_callback_onWsSvcMessage)(void *self, long long sid, long long cid, char *newBuf, int size);
	/*
	连接已断开
	*/
	typedef void (*wss_callback_onWsSvcClose)(void *self, long long sid, long long cid);
    
	extern unsigned char wss_call_onWsSvcReject(wss_callback_t *cb, long long sid, const char *host);
	extern void wss_call_onWsSvcEstablishe(wss_callback_t *cb, long long sid, long long cid);
	extern void wss_call_onWsSvcMessage(wss_callback_t *cb, long long sid, long long cid, char* newBuf, int size);
	extern void wss_call_onWsSvcClose(wss_callback_t *cb, long long sid, long long cid);


    /*
	日志
	*/
	typedef void (*wss_callback_onLog)(const char *logStr);
	extern void wss_call_onLog(wss_callback_onLog onlog, const char *logStr);

#ifdef __cplusplus
}
#endif

#endif //!__FRAMEWORK_NETYPEDEF_H__