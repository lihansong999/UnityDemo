#include "stdafx.h"
#include "WsGoSvcManage.h"
#include "NetworkTools.h"
#include "../LogicalMain.h"
#include "../LogicalHandler/QuickHandler.h"

FW_NS_USEING;


static WsGoSvcManage * _WsGoSvcManage = nullptr;

WsGoSvcManage::WsGoSvcManage()
{
	_WsGoSvcManage = this;
}

WsGoSvcManage::~WsGoSvcManage()
{
	this->Stop();
	_WsGoSvcManage = nullptr;
}

WsGoSvcManage * WsGoSvcManage::GetInstance()
{
	return _WsGoSvcManage;
}

bool WsGoSvcManage::_onWsGoSvcReject(const char* host)
{
	return false;
}

void WsGoSvcManage::_onWsGoSvcOverloaded()
{
	FW_LOG_WRNING("%s", "无法接受连接，服务器已达到最大连接数。");
}

void WsGoSvcManage::_onWsGoSvcEstablishe(const FWNetMatch & netMatch)
{
	if (!LogicalMain::GetInstance()->GetSvcNetMatchesLRU()->Add(SCSNMT_WSGO, netMatch))
	{
		FW_LOG_ERROR("%s", "严重的框架BUG。");
	}
}

void WsGoSvcManage::_onWsGoSvcMessage(const FWNetMatch & netMatch, char * newBuf, unsigned int size)
{
	SvcNetMatch svcNetMatch(SCSNMT_WSGO, netMatch);
	if (!LogicalMain::GetInstance()->GetSvcNetMatchesLRU()->Find(svcNetMatch))
	{
		FW_LOG_ERROR("%s", "严重的框架BUG。");
	}
	FWLogicDataGeneral* logicDataGeneral = new FWLogicDataGeneral();
	logicDataGeneral->PushBackValue<int>(QUICK_H_CLIENT_MSG_ARRIVED);
	logicDataGeneral->PushBackValue<SvcNetMatch>(svcNetMatch);
	logicDataGeneral->PushBackValue<unsigned int>(size);
	logicDataGeneral->PushBackValue<char*>(newBuf);
	QuickHandler::GetInstance()->PushLogicData(logicDataGeneral);
}

void WsGoSvcManage::_onWsGoSvcClose(const FWNetMatch & netMatch)
{
	SvcNetMatch svcNetMatch(SCSNMT_WSGO, netMatch);
	if (!LogicalMain::GetInstance()->GetSvcNetMatchesLRU()->Remove(svcNetMatch))
	{
		FW_LOG_ERROR("%s", "严重的框架BUG。");
	}
	FWLogicDataGeneral* logicDataGeneral = new FWLogicDataGeneral();
	logicDataGeneral->PushBackValue<int>(QUICK_H_CLIENT_SOCKET_CLOSE);
	logicDataGeneral->PushBackValue<SvcNetMatch>(svcNetMatch);
	QuickHandler::GetInstance()->PushLogicData(logicDataGeneral);
}