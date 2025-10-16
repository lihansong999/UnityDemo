#include "stdafx.h"
#include "LogicalMain.h"
#include "LogicalNetwork/NetworkTools.h"
#include "timeapi.h"

FW_NS_USEING;

static LogicalMain * _LogicalMain = nullptr;

LogicalMain::LogicalMain()
	:_running(false)
	, _logicalID(0)
	, _tcpPort(0)
	, _wsPort(0)
	, _tcpMaxClients(0)
	, _wsMaxClients(0)
{
	_LogicalMain = this;

	timeBeginPeriod(1);
}

LogicalMain::~LogicalMain()
{
	this->Stop();
	_LogicalMain = nullptr;
}

LogicalMain * LogicalMain::GetInstance()
{
	return _LogicalMain;
}

bool LogicalMain::Start()
{
	if (_running)
	{
		FW_LOG_ERROR("%s", "�ظ� Start");
		return false;
	}
	_running = true;
	
	FW_PRINT("%s", "��ʼ�߼�����������...");

	FWData fileData = FWFileIO::GetInstance()->LoadFileFromModuleFileDirectory("LogicalConfig/RunConfig.json");
	if (fileData.IsNull())
	{
		FW_LOG_ERROR("%s", "�޷���ȡ�߼��������ļ���");
		_running = false;
		return false;
	}
	rapidjson::GenericDocument<rapidjson::UTF8<> > configDocument;
	configDocument.Parse<rapidjson::kParseDefaultFlags, rapidjson::UTF8<> >(fileData.GetPointer(), (size_t)fileData.GetSize());
	if (configDocument.HasParseError())
	{
		FW_LOG_ERROR("%s", "�߼��������ļ���������");
		_running = false;
		return false;
	}

	_logicalID = configDocument["LogicalID"].GetInt();
	_tcpPort = configDocument["TcpPort"].GetInt();
	_wsPort = configDocument["WsPort"].GetInt();
	_tcpMaxClients = configDocument["TcpMaxClients"].GetInt();
	_wsMaxClients = configDocument["WsMaxClients"].GetInt();

	_svcNetMatchesLRU.Clear();

	_quickHandle.Start(1);

	_tcpSvcManage.Start((unsigned short)_tcpPort, _tcpMaxClients, 0, FWTcpListenSocket::AF_V46MAPPED);

	_wsGoSvcManage.Start((unsigned short)_wsPort, nullptr, nullptr, _wsMaxClients, FWWsGoSvcManageBase::WSGODP_BINARY, MSG_PACKAGE_MAX);

	_schedule.Start(3);

	_schedule.Add([this](long long msInvl, FW_ID id)->void {
		FWLogicDataGeneral* logicDataGeneral = new FWLogicDataGeneral();
		logicDataGeneral->PushBackValue<int>(QUICK_H_SCHEDULE_30);
		QuickHandler::GetInstance()->PushLogicData(logicDataGeneral); // �������붨ʱ���߼�
		}, 30);

	_schedule.Add([this](long long msInvl, FW_ID id)->void {
		SvcNetMatch svcNetMatches[20];
		int count = _svcNetMatchesLRU.LRU(svcNetMatches, sizeof(svcNetMatches) / sizeof(SvcNetMatch));
		if (count > 0)
		{
			long long current = FW_TIME(); // ϵͳ������ʱ���ܻ�����жϴ����漰��ܵĸĶ����ø��ˣ��Ժ��ٿ��Ǹ�Ϊϵͳ����ʱ�䡣
			FWSvcSocketInfo svcSocketInfo;
			for (int i = 0; i < count; i++)
			{
				// ֻ��tcp������Ҫ�ͻ��˷�����������websocket����ping-pong����ҪӦ�ò�����
				if (svcNetMatches[i].Type == SCSNMT_TCP && GetSvcSocketInfo(svcNetMatches[i], &svcSocketInfo))
				{
					if (current - svcSocketInfo.LastReceivedTime > SERVICE_PACKAGE_RECV_OUT_TIME)
					{
						CloseSvcSocket(svcNetMatches[i]);
					}
				}
			}
		}
		}, 8000);

	FW_PRINT("%s", "�߼�������������ɡ�");
	
	return true;
}

void LogicalMain::Stop()
{
	if (!_running)
	{
		return;
	}
	_running = false;

	FW_PRINT("%s", "��ʼ�߼����رչ���...");

	_schedule.Stop();

	_tcpSvcManage.Stop();

	_wsGoSvcManage.Stop();

	_quickHandle.Stop();

	FW_PRINT("%s", "�߼����رչ�����ɡ�");
}

int LogicalMain::GetLogicalID()
{
	return _logicalID;
}

int LogicalMain::GetTcpPort()
{
	return _tcpPort;
}

int LogicalMain::GetWsPort()
{
	return _wsPort;
}

int LogicalMain::GetTcpMaxClients()
{
	return _tcpMaxClients;
}

int LogicalMain::GetWsMaxClients()
{
	return _wsMaxClients;
}

Schedule * LogicalMain::GetSchedule()
{
	return &_schedule;
}

SvcNetMatchesLRU* LogicalMain::GetSvcNetMatchesLRU()
{
	return &_svcNetMatchesLRU;
}