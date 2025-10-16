#pragma once
#include "../LogicalBase/Local.h"
#include "../LogicalBase/TypeDefine.h"


class WsGoSvcManage : public Framework::FWWsGoSvcManageBase
{
public:
	WsGoSvcManage();
	~WsGoSvcManage();
	
	static WsGoSvcManage * GetInstance();

protected:
	virtual bool _onWsGoSvcReject(const char* host) override;

	virtual void _onWsGoSvcOverloaded() override;

	virtual void _onWsGoSvcEstablishe(const Framework::FWNetMatch & netMatch) override;

	virtual void _onWsGoSvcMessage(const Framework::FWNetMatch & netMatch, char * newBuf, unsigned int size) override;

	virtual void _onWsGoSvcClose(const Framework::FWNetMatch & netMatch) override;

private:

};