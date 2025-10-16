#ifndef __FRAMEWORK_SVCTYPEDEF_H__
#define __FRAMEWORK_SVCTYPEDEF_H__

#include "../FWNetTypeDef.h"

FW_NS_BEGIN


struct FW_DLL FWSvcSocketInfo {
	FWSvcSocketInfo();
	/*
	Ip46
	在 FWWsGoSvcManageBase 类中格式为golang net/http 包中的 RemoteAddr，格式一般为 "IP:port"，超过sizeof(Ip46)-1将被截断
	如果使用 FWTcpListenSocket::AF_V46MAPPED 参考 https://tools.ietf.org/html/rfc2553#section-3.7
	*/
	char Ip46[64];
	/*
	在 FWWsGoSvcManageBase 类中该值无效
	*/
	unsigned short Port;
	/*
	FW_TIME()，在 FWWsGoSvcManageBase 类中该值为 FWWsGoSvcManageBase::GoTime，在目前设计中 FW_TIME() 与 FWWsGoSvcManageBase::GoTime 同一时刻的返回值相同
	*/
	long long InitTime;
	/*
	FW_TIME()，在 FWWsGoSvcManageBase 类中该值为 FWWsGoSvcManageBase::GoTime，在目前设计中 FW_TIME() 与 FWWsGoSvcManageBase::GoTime 同一时刻的返回值相同
	*/
	long long LastReceivedTime;
};


FW_NS_END


#endif //!__FRAMEWORK_SVCTYPEDEF_H__