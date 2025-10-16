#include "stdafx.h"
#include "QuickHandler.h"
#include "../LogicalMain.h"
#include "../LogicalNetwork/NetworkTools.h"
#include "../LogicalNetwork/TcpSvcManage.h"
#include "../LogicalNetwork/WsGoSvcManage.h"

FW_NS_USEING;

std::atomic<EntityID_t> s_IncrementEntityID = 1001;

static QuickHandler * _QuickHandler = nullptr;

QuickHandler::QuickHandler()
{
	_QuickHandler = this;

	_ground.set(Vec3(0.0f, 1.0f, 0.0f), 0.0f);
}

QuickHandler::~QuickHandler()
{
	this->Stop();
	_QuickHandler = nullptr;

	for (auto it = _flyEffectInfos.begin(); _flyEffectInfos.end() != it;) {
		delete it->second;
	}
	_flyEffectInfos.clear();
}

QuickHandler * QuickHandler::GetInstance()
{
	return _QuickHandler;
}

void QuickHandler::_deleteLogicData(FWLogicDataBase * dataBase)
{
	delete dataBase;
}

void QuickHandler::_onLogicDataHandler(FWLogicDataBase * dataBase, const char * threadID, void * customArg)
{
	FWLogicDataGeneral * logicDataGeneral = dynamic_cast<FWLogicDataGeneral*>(dataBase);
	if (!logicDataGeneral)
	{
		return;
	}
	switch ((QuickHandlerType)logicDataGeneral->PopFrontValue<int>())
	{
	case QUICK_H_SCHEDULE_30:
	{
		_schedule30();
		break;
	}
	case QUICK_H_CLIENT_MSG_ARRIVED:
	{
		SvcNetMatch svcNetMatch = logicDataGeneral->PopFrontValue<SvcNetMatch>();
		unsigned int msgSize = logicDataGeneral->PopFrontValue<unsigned int>();
		char* msgData = logicDataGeneral->PopFrontValue<char*>();
		long long processResult = -1;
		if (CSMsgSizeMatch(msgData, msgSize))
		{
			MsgHead* pMsgHead = (MsgHead*)msgData;
			unsigned char msgType = pMsgHead->type;
			if (msgType == MSG_TYPE_LOGIN)
			{
				processResult = _loginHandler((MsgCSLogin*)msgData, svcNetMatch);
			}
			else
			{
				PlayerInfo* pPlayerInfo;
				if (_findPlayer(svcNetMatch, &pPlayerInfo))
				{
					switch (msgType)
					{
					case MSG_TYPE_HEARTBEAT:
						processResult = _heartbeatHandler((MsgCSHeartbeat*)msgData, svcNetMatch, pPlayerInfo);
						break;
					case MSG_TYPE_LOGOUT: // �ǳ��߼��ȶ��ߺ���
						break;
					case MSG_TYPE_SYNCTIME:
						processResult = _syncTimeHandler((MsgCSSyncTime*)msgData, svcNetMatch, pPlayerInfo);
						break;
					case MSG_TYPE_SELFADDGAME:
						processResult = _selfAddGameHandler((MsgCSAddGame*)msgData, svcNetMatch, pPlayerInfo);
						break;
					case MSG_TYPE_PLAYER_FRAME:
						processResult = _frameHandler((MsgCSFrame*)msgData, svcNetMatch, pPlayerInfo);
						break;
					case MSG_TYPE_FLY_EF_FRAME:
						processResult = _flyEffectFrameHandler((MsgCSFlyEffectFrame*)msgData, svcNetMatch, pPlayerInfo);
						break;
					}
				}
			}
		}
		if (svcNetMatch.Type == SCSNMT_TCP)
		{
			FW_FREE(msgData);
		}
		else if (svcNetMatch.Type == SCSNMT_WSGO)
		{
			WsGoSvcManage::GoFree(msgData);
		}
		else
		{
			FW_ASSERT(false);
		}
		if (processResult == -1)
		{
			CloseSvcSocket(svcNetMatch);
		}
		else if (processResult > 0)
		{
			long long invl = (processResult < DELAY_CLOSE_SOCKET_TIME_MAX) ? (processResult) : (DELAY_CLOSE_SOCKET_TIME_MAX);
			LogicalMain::GetInstance()->GetSchedule()->Add([svcNetMatch](long long invl, Framework::FW_ID id)->void {
					CloseSvcSocket(svcNetMatch);
				}, invl, 1, true);
		}
	}
		break;
	case QUICK_H_CLIENT_SOCKET_CLOSE:
	{
		SvcNetMatch svcNetMatch = logicDataGeneral->PopFrontValue<SvcNetMatch>();
		PlayerInfo* pPlayerInfo;
		if (_findPlayer(svcNetMatch, &pPlayerInfo))
		{
			_logoutHandler(svcNetMatch, pPlayerInfo);
		}
	}
		break;
	default:
		break;
	}
}

bool QuickHandler::_createPlayer(const SvcNetMatch& svcNetMatch, PlayerInfo** out)
{
	*out = nullptr;
	if (_onlinePlayer.find(svcNetMatch) == _onlinePlayer.end())
	{
		*out = new PlayerInfo(++s_IncrementEntityID, svcNetMatch);
		_onlinePlayer[svcNetMatch] = *out;
		_playerInfos[(*out)->id] = *out;
		return true;
	}
	return false;
}

bool QuickHandler::_findPlayer(const SvcNetMatch& svcNetMatch, PlayerInfo** out)
{
	auto it = _onlinePlayer.find(svcNetMatch);
	if (it != _onlinePlayer.end()) {
		*out = it->second;
		return true;
	}
	return false;
}

bool QuickHandler::_findPlayer(EntityID_t id, PlayerInfo** out)
{
	auto it = _playerInfos.find(id);
	if (it != _playerInfos.end()) {
		*out = it->second;
		return true;
	}
	return false;
}

void QuickHandler::_removePlayer(const SvcNetMatch& svcNetMatch)
{
	auto it1 = _onlinePlayer.find(svcNetMatch);
	if (it1 != _onlinePlayer.end()) {
		auto it2 = _playerInfos.find(it1->second->id);
		if (it2 != _playerInfos.end()) {
			_playerInfos.erase(it2);
		}
		delete it1->second;
		_onlinePlayer.erase(it1);
	}
}

void QuickHandler::_removePlayer(EntityID_t id)
{
	auto it1 = _playerInfos.find(id);
	if (it1 != _playerInfos.end()) {
		auto it2 = _onlinePlayer.find(it1->second->svcNetMatch);
		if (it2 != _onlinePlayer.end()) {
			_onlinePlayer.erase(it2);
		}
		PlayerInfo* temp = it1->second;
		_playerInfos.erase(it1);
		delete temp;
	}
}

void QuickHandler::_broadcastMsgGameingPlayers(const char* data, int size, EntityID_t ignore)
{
	for (auto& it : _onlinePlayer)
		if (it.second->gameing && it.second->id != ignore)
			SendDataSvcSocket(it.first, data, size);
}

bool QuickHandler::_createFlyEffect(EntityID_t creatorID, FlyEffectInfo** out)
{
	*out = new FlyEffectInfo(++s_IncrementEntityID, creatorID);
	_flyEffectInfos[(*out)->id] = *out;
	return true;
}

void QuickHandler::_removeFlyEffect(EntityID_t id)
{
	auto it = _flyEffectInfos.find(id);
	if (it != _flyEffectInfos.end()) {
		FlyEffectInfo* temp = it->second;
		_flyEffectInfos.erase(it);
		delete temp;
	}
}

void QuickHandler::_schedule30()
{
	_flyEffectCalculateFromSchedule30Handler();
}

long long QuickHandler::_heartbeatHandler(MsgCSHeartbeat* msgData, const SvcNetMatch& svcNetMatch, PlayerInfo* playerInfo)
{
	MsgSCHeartbeat res;
	SendDataSvcSocket(svcNetMatch, (char*)&res, sizeof(res));
	return 0;
}

long long QuickHandler::_loginHandler(MsgCSLogin* msgData, const SvcNetMatch& svcNetMatch)
{
	PlayerInfo* playerInfo;
	if (!_createPlayer(svcNetMatch, &playerInfo))
	{
		return -1;
	}
	MsgSCLogin res;
	res.selfEntityID = playerInfo->id;
	SendDataSvcSocket(svcNetMatch, (char*)&res, sizeof(res));
	return 0;
}

void QuickHandler::_logoutHandler(const SvcNetMatch& svcNetMatch, PlayerInfo* playerInfo)
{
	MsgSCLogout res;
	res.outEntityID = playerInfo->id;
	if (playerInfo->gameing)
	{
		_broadcastMsgGameingPlayers((char*)&res, sizeof(res), playerInfo->id);
	}
	_removePlayer(svcNetMatch);
}

long long QuickHandler::_syncTimeHandler(MsgCSSyncTime* msgData, const SvcNetMatch& svcNetMatch, PlayerInfo* playerInfo)
{
	MsgSCSyncTime res;
	res.reqTime = msgData->reqTime;
	res.serverTime = FW_TIME2();
	SendDataSvcSocket(svcNetMatch, (char*)&res, sizeof(res));
	return 0;
}

long long QuickHandler::_selfAddGameHandler(MsgCSAddGame* msgData, const SvcNetMatch& svcNetMatch, PlayerInfo* playerInfo)
{
	playerInfo->gameing = true;
	MsgSCAddGame res;
	res.birthPoint = rand() % 8; // ���һ��������
	SendDataSvcSocket(svcNetMatch, (char*)&res, sizeof(res));

	// �㲥�����Ѽ�����ҵ����һ֡����
	MsgSCFrame res2;
	for (auto& it : _onlinePlayer) 
	{
		if (it.second->gameing && it.second->id != playerInfo->id && it.second->lastFrame.curTime > 0)
		{
			res2.frameEntityID = it.second->id;
			res2.frameInfo = it.second->lastFrame;
			SendDataSvcSocket(svcNetMatch, (char*)&res2, sizeof(res2));
		}
	}

	return 0;
}

long long QuickHandler::_frameHandler(MsgCSFrame* msgData, const SvcNetMatch& svcNetMatch, PlayerInfo* playerInfo)
{
	if (!playerInfo->gameing)
		return -1;

	auto svrTime = FW_TIME2();
	if (msgData->frameInfo.curTime > svrTime)
		msgData->frameInfo.curTime = svrTime;	// ǿ������

	playerInfo->lastFrame = msgData->frameInfo;

	MsgSCFrame res;
	res.frameEntityID = playerInfo->id;
	res.frameInfo = msgData->frameInfo;
	_broadcastMsgGameingPlayers((char*)&res, sizeof(res), playerInfo->id);

	return 0;
}

long long QuickHandler::_flyEffectFrameHandler(MsgCSFlyEffectFrame* msgData, const SvcNetMatch& svcNetMatch, PlayerInfo* playerInfo)
{
	FlyEffectInfo* flyEffectInfo;
	if (!_createFlyEffect(playerInfo->id, &flyEffectInfo))
	{
		return 0;
	}
	
	auto svrTime = FW_TIME2();
	if (msgData->curTime > svrTime)
		msgData->curTime = svrTime;	// ǿ������

	flyEffectInfo->launchTime = msgData->curTime;
	flyEffectInfo->lastCalcTime = msgData->curTime;
	flyEffectInfo->launchPos.set(MGS_FUN_I2F(msgData->positionX), MGS_FUN_I2F(msgData->positionY), MGS_FUN_I2F(msgData->positionZ));
	flyEffectInfo->velocity.set(MGS_FUN_I2F(msgData->velocityX), MGS_FUN_I2F(msgData->velocityY), MGS_FUN_I2F(msgData->velocityZ));

	// �㲥��ʼ����
	MsgSCFlyEffectFrame res;
	res.frameEntityID = flyEffectInfo->id;
	res.frameInfo.curTime = msgData->curTime;
	res.frameInfo.infoType = 1;
	res.frameInfo.positionX = msgData->positionX;
	res.frameInfo.positionY = msgData->positionY;
	res.frameInfo.positionZ = msgData->positionZ;
	res.frameInfo.velocityX = msgData->velocityX;
	res.frameInfo.velocityY = msgData->velocityY;
	res.frameInfo.velocityZ = msgData->velocityZ;
	_broadcastMsgGameingPlayers((char*)&res, sizeof(res), 0);

	if (!_flyEffectCalculateSingle(flyEffectInfo))
	{
		_removeFlyEffect(flyEffectInfo->id);
	}

	return 0;
}

void QuickHandler::_flyEffectCalculateFromSchedule30Handler()
{
	for (auto it = _flyEffectInfos.begin(); _flyEffectInfos.end() != it;)
	{
		if (!_flyEffectCalculateSingle(it->second))
		{
			FlyEffectInfo* temp = it->second;
			it = _flyEffectInfos.erase(it);
			delete temp;
		}
		else
		{
			it++;
		}
	}
}

bool QuickHandler::_flyEffectCalculateSingle(FlyEffectInfo* flyEffectInfo)
{
	// �ƻ����Ϊ��
	// ����ʱ�㲥�������˷�����Ϣ
	// ֮���ɿͻ��˱���ģ�⣬������Ҳͬʱÿ30����һ�μ��㣬ÿ�μ��㲽��Ϊ60���루��ǰ���㣩��Ŀǰ��ɫ�ܶ��ٶ���ÿ��10�ף����ܷ�����100��ÿ�룬�����������Ӧ����0.6�ף�������һ����λ��
	// ���������ɷ������㲥������֮ǰ���ɿͻ��˱�������ģ�⣬�ͻ��˿ɲ��ñȷ����������16.67����һ�μ��㣬ÿ�β���Ҳͬ����16.67����
	// ������Ҫ������������ʲô�ɷ���������
	// ��������������������1.������һ��������壬2.����������

	long long curTime = Framework::FW_TIME2();

	long long st = (curTime - flyEffectInfo->lastCalcTime > 0) ? (flyEffectInfo->lastCalcTime) : (curTime); // ȡ�������Ϊ���μ�������
	long long nt = curTime + 60;

	float ldt = (st - flyEffectInfo->launchTime) / 1000.0f;
	float cdt = (nt - flyEffectInfo->launchTime) / 1000.0f; // +60��Ϊ��ÿ�μ��㵽60����֮��

	Vec3 lpos = flyEffectInfo->launchPos + flyEffectInfo->velocity * ldt;
	Vec3 cpos = flyEffectInfo->launchPos + flyEffectInfo->velocity * cdt;

	Capsule capsule(lpos, cpos, FlyEffectRadius);

	Capsule tempCapsule;
	for (auto& it : _onlinePlayer)
	{
		if (it.second->gameing && it.second->id != flyEffectInfo->creatorID)
		{
			tempCapsule.set(Vec3(MGS_FUN_I2F(it.second->lastFrame.positionX), MGS_FUN_I2F(it.second->lastFrame.positionY), MGS_FUN_I2F(it.second->lastFrame.positionZ)), 0.35f, 1.8f);
			float t = 0.0f;
			Vec3 cp;
			if (capsule.intersects(tempCapsule, &t, nullptr, &cp, nullptr))
			{
				// �㲥�������
				MsgSCFlyEffectFrame res;
				res.frameEntityID = flyEffectInfo->id;
				res.frameInfo.curTime = st + (long long)((nt - st) * t);
				res.frameInfo.infoType = 2;
				res.frameInfo.positionX = MGS_FUN_F2I(cp.x);
				res.frameInfo.positionY = MGS_FUN_F2I(cp.y);
				res.frameInfo.positionZ = MGS_FUN_F2I(cp.z);
				res.frameInfo.velocityX = MGS_FUN_F2I(flyEffectInfo->velocity.x);
				res.frameInfo.velocityY = MGS_FUN_F2I(flyEffectInfo->velocity.y);
				res.frameInfo.velocityZ = MGS_FUN_F2I(flyEffectInfo->velocity.z);
				_broadcastMsgGameingPlayers((char*)&res, sizeof(res), 0);
				return false;
			}
		}
	}

	float t = 0.0f;
	Vec3 cp;
	if (capsule.intersectsOrBehindPlane(_ground, &t, &cp) || _gameMap.bvhIntersects_Capsule_1(capsule, &t, &cp))
	{
		// �㲥���е������
		MsgSCFlyEffectFrame res;
		res.frameEntityID = flyEffectInfo->id;
		res.frameInfo.curTime = st + (long long)((nt - st) * t);
		res.frameInfo.infoType = 2;
		res.frameInfo.positionX = MGS_FUN_F2I(cp.x);
		res.frameInfo.positionY = MGS_FUN_F2I(cp.y);
		res.frameInfo.positionZ = MGS_FUN_F2I(cp.z);
		res.frameInfo.velocityX = MGS_FUN_F2I(flyEffectInfo->velocity.x);
		res.frameInfo.velocityY = MGS_FUN_F2I(flyEffectInfo->velocity.y);
		res.frameInfo.velocityZ = MGS_FUN_F2I(flyEffectInfo->velocity.z);
		_broadcastMsgGameingPlayers((char*)&res, sizeof(res), 0);
		return false;
	}

	flyEffectInfo->lastCalcTime = curTime + 30; // �´μ���Ŀ�ʼʱ���

	if (flyEffectInfo->lastCalcTime - flyEffectInfo->launchTime >= (FlyEffectRange / FlyEffectSpeed) * 1000LL)
	{
		// �㲥�������
		MsgSCFlyEffectFrame res;
		res.frameEntityID = flyEffectInfo->id;
		res.frameInfo.curTime = nt;
		res.frameInfo.infoType = 3;
		res.frameInfo.positionX = MGS_FUN_F2I(cp.x);
		res.frameInfo.positionY = MGS_FUN_F2I(cp.y);
		res.frameInfo.positionZ = MGS_FUN_F2I(cp.z);
		res.frameInfo.velocityX = MGS_FUN_F2I(flyEffectInfo->velocity.x);
		res.frameInfo.velocityY = MGS_FUN_F2I(flyEffectInfo->velocity.y);
		res.frameInfo.velocityZ = MGS_FUN_F2I(flyEffectInfo->velocity.z);
		_broadcastMsgGameingPlayers((char*)&res, sizeof(res), 0);
		return false;
	}

	return true;
}