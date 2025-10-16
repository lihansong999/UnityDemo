#include "stdafx.h"
#include "NetworkTools.h"
#include "TcpSvcManage.h"
#include "WsGoSvcManage.h"

FW_NS_USEING;


const long long SERVICE_PACKAGE_RECV_OUT_TIME = 20000LL;
const long long DELAY_CLOSE_SOCKET_TIME_FOR_SENT = 150LL;
const long long DELAY_CLOSE_SOCKET_TIME_MAX = 1000LL;

SvcNetMatch::SvcNetMatch()
	:Type(SCSNMT_MAX)
{

}

SvcNetMatch::SvcNetMatch(SvcNetMatchType type, const FWNetMatch& netMatch)
	:Type(type)
	, NetMatch(netMatch)
{

}

SvcNetMatch::SvcNetMatch(int type, unsigned int index, FW_NET_ACTIVITY_ID activityID)
	:Type((SvcNetMatchType)type)
	, NetMatch(index, activityID)
{

}

bool SvcNetMatch::operator<(const SvcNetMatch& other) const
{
	if (Type != other.Type)
	{
		return Type < other.Type;
	}
	return NetMatch < other.NetMatch;
}

bool SvcNetMatch::operator==(const SvcNetMatch& other) const
{
	return Type == other.Type && NetMatch == other.NetMatch;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

SvcNetMatchesLRU::SvcNetMatchesLRU()
{
	_svcNetMatchesSet = new FWLinkedSet<SvcNetMatch>(true);
}

SvcNetMatchesLRU::~SvcNetMatchesLRU()
{
	delete _svcNetMatchesSet;
}

bool SvcNetMatchesLRU::Add(const SvcNetMatch& svcNetMatch)
{
	FWLockScopeController lsc(&_svcNetMatchesContainerLock);
	if (!_svcNetMatchesSet->Insert(svcNetMatch))
	{
		return false;
	}
	return true;
}

bool SvcNetMatchesLRU::Add(SvcNetMatchType type, const FWNetMatch& netMatch)
{
	return Add(SvcNetMatch(type, netMatch));
}

bool SvcNetMatchesLRU::Find(const SvcNetMatch& svcNetMatch)
{
	FWLockScopeController lsc(&_svcNetMatchesContainerLock);
	FWLinkedSet<SvcNetMatch>::Iterator it = _svcNetMatchesSet->Find(svcNetMatch);
	if (it != _svcNetMatchesSet->End())
	{
		return true;
	}
	return false;
}

bool SvcNetMatchesLRU::Find(SvcNetMatchType type, const FWNetMatch& netMatch)
{
	return Find(SvcNetMatch(type, netMatch));
}

bool SvcNetMatchesLRU::Remove(const SvcNetMatch& svcNetMatch)
{
	FWLockScopeController lsc(&_svcNetMatchesContainerLock);
	return _svcNetMatchesSet->Erase(svcNetMatch) == 1;
}

bool SvcNetMatchesLRU::Remove(SvcNetMatchType type, const FWNetMatch& netMatch)
{
	return Find(SvcNetMatch(type, netMatch));
}

void SvcNetMatchesLRU::Clear()
{
	FWLockScopeController lsc(&_svcNetMatchesContainerLock);
	_svcNetMatchesSet->Clear();
}

int SvcNetMatchesLRU::LRU(SvcNetMatch* out_svcNetMatches, int count)
{
	FWLockScopeController lsc(&_svcNetMatchesContainerLock);
	if (!out_svcNetMatches || count <= 0)
	{
		return 0;
	}
	int index = 0;
	for (FWLinkedSet<SvcNetMatch>::Iterator it = _svcNetMatchesSet->Begin(); it != _svcNetMatchesSet->End(); it++)
	{
		out_svcNetMatches[index] = *it;
		if (++index == count)
			break;
	}
	return index;
}

bool SendDataSvcSocket(const SvcNetMatch & svcNetMatch, const char * pData, int size)
{
	if (svcNetMatch.Type == SCSNMT_TCP)
	{
		if (size > MSG_PACKAGE_MAX)
		{
			FW_LOG_ERROR("%s", "size is too large");
			return false;
		}
		static char s_TcpBuff[MSG_PACKAGE_MAX + sizeof(int)];	// 目前不会跨线程调用，暂时可以这样写
		*((int*)s_TcpBuff) = size;
		memcpy(s_TcpBuff + sizeof(int), pData, size);
		return TcpSvcManage::GetInstance()->SendDataTcpSvcSocket(svcNetMatch.NetMatch, s_TcpBuff, size + sizeof(int));
	}
	else if (svcNetMatch.Type == SCSNMT_WSGO)
	{
		return WsGoSvcManage::GetInstance()->SendDataWsGoSvcSocket(svcNetMatch.NetMatch, pData, size);
	}
	return false;
}

void SendDataSvcSocketBatch(const SvcNetMatch * svcNetMatches, unsigned int count, const char * pData, int size)
{
	if (!pData || size <= 0)
	{
		return;
	}
	if (!svcNetMatches || count == 0)
	{
		return;
	}
	for (unsigned int i = 0; i < count; i++)
	{
		SendDataSvcSocket(svcNetMatches[i], pData, size);
	}
}

void CloseSvcSocket(const SvcNetMatch & svcNetMatch)
{
	if (svcNetMatch.Type == SCSNMT_TCP)
	{
		TcpSvcManage::GetInstance()->CloseTcpSvcSocket(svcNetMatch.NetMatch);
	}
	else if (svcNetMatch.Type == SCSNMT_WSGO)
	{
		WsGoSvcManage::GetInstance()->CloseWsGoSvcSocket(svcNetMatch.NetMatch);
	}
}

void CloseSvcSocketBatch(const SvcNetMatch * svcNetMatches, unsigned int count)
{
	if (!svcNetMatches || count == 0)
	{
		return;
	}
	for (unsigned int i = 0; i < count; i++)
	{
		CloseSvcSocket(svcNetMatches[i]);
	}
}

bool GetSvcSocketInfo(const SvcNetMatch & svcNetMatch, FWSvcSocketInfo * out_svcSocketInfo)
{
	if (svcNetMatch.Type == SCSNMT_TCP)
	{
		return TcpSvcManage::GetInstance()->GetTcpSvcSocketInfo(svcNetMatch.NetMatch, out_svcSocketInfo);
	}
	else if (svcNetMatch.Type == SCSNMT_WSGO)
	{
		return WsGoSvcManage::GetInstance()->GetWsGoSvcSocketInfo(svcNetMatch.NetMatch, out_svcSocketInfo);
	}
	return false;
}

int GetSvcConnectedCount()
{
	return TcpSvcManage::GetInstance()->GetConnectedCount() + WsGoSvcManage::GetInstance()->GetConnectedCount();
}