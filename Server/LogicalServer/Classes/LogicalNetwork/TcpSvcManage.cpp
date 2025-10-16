#include "stdafx.h"
#include "TcpSvcManage.h"
#include "NetworkTools.h"
#include "../LogicalMain.h"
#include "../LogicalHandler/QuickHandler.h"

FW_NS_USEING;


static TcpSvcManage * _TcpSvcManage = nullptr;

TcpSvcManage::TcpSvcManage()
{
	_TcpSvcManage = this;
}

TcpSvcManage::~TcpSvcManage()
{
	this->Stop();
	_TcpSvcManage = nullptr;
}

TcpSvcManage * TcpSvcManage::GetInstance()
{
	return _TcpSvcManage;
}

bool TcpSvcManage::_onTcpSvcReject(const char * ip46, unsigned short port)
{
	return false;
}

void TcpSvcManage::_onTcpSvcOverloaded()
{
	FW_LOG_WRNING("%s", "无法分配Socket，服务器已达到最大连接数。");
}

void TcpSvcManage::_onTcpSvcAccepted(const FWNetMatch & netMatch, const FWSvcSocketInfo & svcSocketInfo)
{
	if (!LogicalMain::GetInstance()->GetSvcNetMatchesLRU()->Add(SCSNMT_TCP, netMatch))
	{
		FW_LOG_ERROR("%s", "严重的框架BUG。");
	}
}

unsigned int TcpSvcManage::_onTcpSvcRecvTest(const char * buf, unsigned int size)
{
	int validCount = 0;
	unsigned int validSize = 0;
	for (; validCount < INT_MAX; validCount++)
	{
		if (size < validSize + (int)sizeof(int))
			break;
		int s = *((int*)(buf + validSize));
		if (s <= 0 || s > MSG_PACKAGE_MAX)
			return -1;
		int vs = s + sizeof(int);
		if (size < validSize + vs)
			break;
		validSize += vs;
	}
	return validSize;
}

void TcpSvcManage::_onTcpSvcRecvCompleted(const FWNetMatch & netMatch, const FWSvcSocketInfo & svcSocketInfo, char * newBuf, unsigned int size)
{
	SvcNetMatch svcNetMatch(SCSNMT_TCP, netMatch);
	if (!LogicalMain::GetInstance()->GetSvcNetMatchesLRU()->Find(svcNetMatch))
	{
		FW_LOG_ERROR("%s", "严重的框架BUG。");
	}
	char* src_buff = newBuf;
	while (size > 0)
	{
		int s = *((int*)newBuf);
		char* buf = (char*)FW_MALLOC(s);
		memcpy(buf, newBuf + sizeof(int), s);
		FWLogicDataGeneral* logicDataGeneral = new FWLogicDataGeneral();
		logicDataGeneral->PushBackValue<int>(QUICK_H_CLIENT_MSG_ARRIVED);
		logicDataGeneral->PushBackValue<SvcNetMatch>(svcNetMatch);
		logicDataGeneral->PushBackValue<unsigned int>(s);
		logicDataGeneral->PushBackValue<char*>(buf);
		QuickHandler::GetInstance()->PushLogicData(logicDataGeneral);
		newBuf += s + sizeof(int);
		size -= s + sizeof(int);
	}
	FW_FREE(src_buff);
}

void TcpSvcManage::_onTcpSvcClose(const FWNetMatch & netMatch, const FWSvcSocketInfo & svcSocketInfo)
{
	SvcNetMatch svcNetMatch(SCSNMT_TCP, netMatch);
	if (!LogicalMain::GetInstance()->GetSvcNetMatchesLRU()->Remove(svcNetMatch))
	{
		FW_LOG_ERROR("%s", "严重的框架BUG。");
	}
	FWLogicDataGeneral* logicDataGeneral = new FWLogicDataGeneral();
	logicDataGeneral->PushBackValue<int>(QUICK_H_CLIENT_SOCKET_CLOSE);
	logicDataGeneral->PushBackValue<SvcNetMatch>(svcNetMatch);
	QuickHandler::GetInstance()->PushLogicData(logicDataGeneral);
}