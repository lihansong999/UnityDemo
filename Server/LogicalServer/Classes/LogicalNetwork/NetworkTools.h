#pragma once
#include "../LogicalBase/Local.h"
#include "../LogicalBase/TypeDefine.h"

//���û���յ�һ����������Ϣ���Ͷ���
extern const long long SERVICE_PACKAGE_RECV_OUT_TIME;

//����Send֮���ӳٹر�Socket�ĵȴ�ʱ�䣨�����ܰٷ�֮�ٱ�֤Send��ɣ�
extern const long long DELAY_CLOSE_SOCKET_TIME_FOR_SENT;

//�ӳٹر�Socket�ĵȴ�ʱ�����ֵ
extern const long long DELAY_CLOSE_SOCKET_TIME_MAX;

#define MSG_PACKAGE_MAX 2048

enum SvcNetMatchType
{
	SCSNMT_TCP = 1,	//TCP
	SCSNMT_WSGO,	//WebSocketGolang

	SCSNMT_MAX = INT_MAX,
};

//����NetMatch
struct SvcNetMatch {
	SvcNetMatch();
	SvcNetMatch(SvcNetMatchType type, const Framework::FWNetMatch& netMatch);
	SvcNetMatch(int type, unsigned int index, Framework::FW_NET_ACTIVITY_ID activityID);
	bool operator<(const SvcNetMatch& other) const;
	bool operator==(const SvcNetMatch& other) const;
	SvcNetMatchType			Type;
	Framework::FWNetMatch	NetMatch;
};

class SvcNetMatchesLRU
{
public:
	SvcNetMatchesLRU();
	~SvcNetMatchesLRU();

	bool Add(const SvcNetMatch& svcNetMatch);
	bool Add(SvcNetMatchType type, const Framework::FWNetMatch& netMatch);

	bool Find(const SvcNetMatch& svcNetMatch);
	bool Find(SvcNetMatchType type, const Framework::FWNetMatch& netMatch);

	bool Remove(const SvcNetMatch& svcNetMatch);
	bool Remove(SvcNetMatchType type, const Framework::FWNetMatch& netMatch);

	void Clear();

	int LRU(SvcNetMatch* out_svcNetMatches, int count);

private:
	Framework::FWLockRecursive				_svcNetMatchesContainerLock;
	Framework::FWLinkedSet<SvcNetMatch>*	_svcNetMatchesSet;
};

typedef long long EntityID_t;

bool SendDataSvcSocket(const SvcNetMatch & svcNetMatch, const char * pData, int size);

void SendDataSvcSocketBatch(const SvcNetMatch * svcNetMatches, unsigned int count, const char * pData, int size);

void CloseSvcSocket(const SvcNetMatch & svcNetMatch);

void CloseSvcSocketBatch(const SvcNetMatch * svcNetMatches, unsigned int count);

bool GetSvcSocketInfo(const SvcNetMatch & svcNetMatch, Framework::FWSvcSocketInfo * out_svcSocketInfo);

int GetSvcConnectedCount();