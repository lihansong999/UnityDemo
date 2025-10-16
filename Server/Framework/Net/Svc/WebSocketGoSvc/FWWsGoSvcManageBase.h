#ifndef __FRAMEWORK_WSGOSVCMANAGEBASE_H__
#define __FRAMEWORK_WSGOSVCMANAGEBASE_H__

#include "FWWsGoSvcTypeDef.h"

FW_NS_BEGIN


/*
WebSocket����������
�ɽ������ӣ��Լ��������ӵ��׽���
�̳к�ʹ��
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
	port�������˿ں�
	pattern��·������url��·�����֣���'/'��ͷ��û�пɴ�NULL��
	subprotocol����Э�飬��JavaScript�ͻ���WebSocket(url, protocols)�ĵ�2��������û�пɴ�NULL��
	maxConnectedCount��������������
	dataProtocol��Э����������
	maxMessageSize�����յ���message����󳤶ȣ�����������ȵĻᱻ��Ϊ�Ƿ����ӣ������Ͽ�
	rsize����ȡ��������С����������Ϊ����ÿһ���������յ���Ϣ��
	wsize��д�뻺������С����������Ϊ����ÿһ���������͵���Ϣ��
	sendChannelSize��golang�ķ���Channel��������С����������Ϊ����ͬһʱ�̲��з��͵ĸ�����
	handshakeTimeout��websocket���ֳ�ʱʱ��
	writeWait��д�뷢�ͻ�������ʱʱ��
	pongWait��pong�ĵȴ�ʱ�䣬�����ȴ�ʱ��û���յ�pong����Ϊ�����ѶϿ�
	pingPeriod��ping�ķ��ͼ��ʱ��
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
	certFilePath����Կ�ļ�·��
	keyFilePath��˽Կ�ļ�·��
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
	�����ͷ���golangģ���������c�ڴ�
	�����ͷ�_onWsGoSvcMessage�ص��е�newBuf����
	*/
	static void GoFree(void * pointer);

	/*
	��ȡgolangģ���е�ʱ��
	*/
	static long long GoTime();

	/*
	����golang�̵߳����������ͬʱ����֮ǰ�����ã�Ĭ��ΪCPU������
	�����С��0��ֵ���򲻸ı�����
	*/
	static int GoMaxprocs(int maxProcs);

protected:
	/*
	�Ƿ�ܾ��˴�����Э�飬Ĭ��ʼ�շ���false
	true���ܾ�
	false�����ܾ�
	*/
	virtual bool _onWsGoSvcReject(const char* host);

	/*
	����������Ѵ����ߣ���ʱ�����ӽ����ᱻ�Զ��Ͽ��������ô˺���
	*/
	virtual void _onWsGoSvcOverloaded();

	/*
	����Э�������
	*/
	virtual void _onWsGoSvcEstablishe(const FWNetMatch& netMatch) = 0;

	/*
	���������
	newBuf��message���ݻ�������ʹ����ɺ�������GoFree�ͷţ�����sizeΪ0ʱ��ֻҪnewBuf!=NULL�ͱ������GoFree�ͷţ�
	size��newBuf�е����ݴ�С��֧��0��С��message��
	*/
	virtual void _onWsGoSvcMessage(const FWNetMatch& netMatch, char* newBuf, unsigned int size) = 0;

	/*
	�����ѶϿ�
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