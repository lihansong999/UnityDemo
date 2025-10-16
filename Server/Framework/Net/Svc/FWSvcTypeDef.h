#ifndef __FRAMEWORK_SVCTYPEDEF_H__
#define __FRAMEWORK_SVCTYPEDEF_H__

#include "../FWNetTypeDef.h"

FW_NS_BEGIN


struct FW_DLL FWSvcSocketInfo {
	FWSvcSocketInfo();
	/*
	Ip46
	�� FWWsGoSvcManageBase ���и�ʽΪgolang net/http ���е� RemoteAddr����ʽһ��Ϊ "IP:port"������sizeof(Ip46)-1�����ض�
	���ʹ�� FWTcpListenSocket::AF_V46MAPPED �ο� https://tools.ietf.org/html/rfc2553#section-3.7
	*/
	char Ip46[64];
	/*
	�� FWWsGoSvcManageBase ���и�ֵ��Ч
	*/
	unsigned short Port;
	/*
	FW_TIME()���� FWWsGoSvcManageBase ���и�ֵΪ FWWsGoSvcManageBase::GoTime����Ŀǰ����� FW_TIME() �� FWWsGoSvcManageBase::GoTime ͬһʱ�̵ķ���ֵ��ͬ
	*/
	long long InitTime;
	/*
	FW_TIME()���� FWWsGoSvcManageBase ���и�ֵΪ FWWsGoSvcManageBase::GoTime����Ŀǰ����� FW_TIME() �� FWWsGoSvcManageBase::GoTime ͬһʱ�̵ķ���ֵ��ͬ
	*/
	long long LastReceivedTime;
};


FW_NS_END


#endif //!__FRAMEWORK_SVCTYPEDEF_H__