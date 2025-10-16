#pragma once
#include "LogicalBase/Local.h"
#include "LogicalBase/TypeDefine.h"
#include "LogicalNetwork/TcpSvcManage.h"
#include "LogicalNetwork/WsGoSvcManage.h"
#include "LogicalNetwork/NetworkTools.h"
#include "LogicalHandler/QuickHandler.h"


class LogicalMain
{
public:
	LogicalMain();
	~LogicalMain();

	static LogicalMain * GetInstance();

	bool Start();

	void Stop();

	int GetLogicalID();

	int GetTcpPort();

	int GetWsPort();

	int GetTcpMaxClients();

	int GetWsMaxClients();

	Schedule * GetSchedule();

	SvcNetMatchesLRU* GetSvcNetMatchesLRU();

private:
	volatile bool			_running;
	int						_logicalID;
	int						_tcpPort;
	int						_wsPort;
	int						_tcpMaxClients;
	int						_wsMaxClients;
	Schedule				_schedule;
	TcpSvcManage			_tcpSvcManage;
	WsGoSvcManage			_wsGoSvcManage;
	QuickHandler			_quickHandle;
	SvcNetMatchesLRU		_svcNetMatchesLRU;
};