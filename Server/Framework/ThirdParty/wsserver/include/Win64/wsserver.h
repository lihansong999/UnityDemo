#ifndef __WSSERVER_H__
#define __WSSERVER_H__

#ifdef __cplusplus
extern "C" {
#endif

	/*
	������Ϣ
	*/
	typedef struct {
		char *host;
		long long initTime;
		long long lastReceivedTime;
	}wss_socket_info_t;

	/*
	�ص��ṹ
	*/
	typedef struct {
		void* self;
		void* onWsSvcReject;
		void* onWsSvcEstablishe;
		void* onWsSvcMessage;
		void* onWsSvcClose;
	}wss_callback_t;

	/*
	�Ƿ�ܾ��˴�����Э�飬Ĭ��ʼ�շ���0
	1���ܾ�
	0�����ܾ�
	*/
	typedef unsigned char (*wss_callback_onWsSvcReject)(void *self, long long sid, const char *host);
	/*
	����Э�������
	*/
	typedef void (*wss_callback_onWsSvcEstablishe)(void *self, long long sid, long long cid);
	/*
	���������
	arg4��message���ݻ�������ʹ����ɺ�������GO_FREE�ͷţ�����arg5Ϊ0ʱ��ֻҪarg4!=NULL�ͱ������GO_FREE�ͷţ�
	arg5�����ݴ�С��֧��0��С��message��
	*/
	typedef void (*wss_callback_onWsSvcMessage)(void *self, long long sid, long long cid, char *newBuf, int size);
	/*
	�����ѶϿ�
	*/
	typedef void (*wss_callback_onWsSvcClose)(void *self, long long sid, long long cid);
    
	extern unsigned char wss_call_onWsSvcReject(wss_callback_t *cb, long long sid, const char *host);
	extern void wss_call_onWsSvcEstablishe(wss_callback_t *cb, long long sid, long long cid);
	extern void wss_call_onWsSvcMessage(wss_callback_t *cb, long long sid, long long cid, char* newBuf, int size);
	extern void wss_call_onWsSvcClose(wss_callback_t *cb, long long sid, long long cid);


    /*
	��־
	*/
	typedef void (*wss_callback_onLog)(const char *logStr);
	extern void wss_call_onLog(wss_callback_onLog onlog, const char *logStr);

#ifdef __cplusplus
}
#endif

#endif //!__FRAMEWORK_NETYPEDEF_H__