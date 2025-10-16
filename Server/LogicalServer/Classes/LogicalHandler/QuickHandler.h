#pragma once
#include "../LogicalBase/Local.h"
#include "../LogicalBase/TypeDefine.h"
#include "HandlerTypeDef.h"
#include "../LogicalNetwork/Message/Message.h"
#include "GameMap.h"

struct PlayerInfo {
	PlayerInfo(EntityID_t entityID, SvcNetMatch m)
		:id(entityID),
		svcNetMatch(m),
		gameing(false)
	{}
	const EntityID_t id;
	const SvcNetMatch svcNetMatch;
	bool gameing;
	NetRoleFrame lastFrame;	// 最后一帧
};

#define FlyEffectRadius (0.3f)	// 飞行特效半径
#define FlyEffectRange (500.0f) // 飞行特效射程
#define FlyEffectSpeed (100.0f) // 飞行特效速度（每秒）
struct FlyEffectInfo {
	FlyEffectInfo(EntityID_t entityID, EntityID_t cID)
		:id(entityID),
		creatorID(cID),
		launchTime(0),
		lastCalcTime(0)
	{
	}
	const EntityID_t id;
	const EntityID_t creatorID;
	long long launchTime;
	long long lastCalcTime;
	Framework::Vec3 launchPos;
	Framework::Vec3 velocity;
};

class QuickHandler : public Framework::FWLogicHandlerBase
{
public:
	QuickHandler();
	~QuickHandler();

	static QuickHandler * GetInstance();

protected:
	virtual void _deleteLogicData(Framework::FWLogicDataBase * dataBase) override;

	virtual void _onLogicDataHandler(Framework::FWLogicDataBase * dataBase, const char * threadID, void * customArg) override;

private:
	std::map<SvcNetMatch, PlayerInfo*> _onlinePlayer;
	std::map<EntityID_t, PlayerInfo*> _playerInfos;
	bool _createPlayer(const SvcNetMatch& svcNetMatch, PlayerInfo** out);
	bool _findPlayer(const SvcNetMatch& svcNetMatch, PlayerInfo** out);
	bool _findPlayer(EntityID_t id, PlayerInfo** out);
	void _removePlayer(const SvcNetMatch& svcNetMatch);
	void _removePlayer(EntityID_t id);
	void _broadcastMsgGameingPlayers(const char* data, int size, EntityID_t ignore);

private:
	std::map<EntityID_t, FlyEffectInfo*> _flyEffectInfos;
	bool _createFlyEffect(EntityID_t creatorID, FlyEffectInfo** out);
	void _removeFlyEffect(EntityID_t id);

private:
	void _schedule30();
	long long _heartbeatHandler(MsgCSHeartbeat* msgData, const SvcNetMatch& svcNetMatch, PlayerInfo* playerInfo);
	long long _loginHandler(MsgCSLogin* msgData, const SvcNetMatch& svcNetMatch);
	void _logoutHandler(const SvcNetMatch& svcNetMatch, PlayerInfo* playerInfo);
	long long _syncTimeHandler(MsgCSSyncTime* msgData, const SvcNetMatch& svcNetMatch, PlayerInfo* playerInfo);
	long long _selfAddGameHandler(MsgCSAddGame* msgData, const SvcNetMatch& svcNetMatch, PlayerInfo* playerInfo);
	long long _frameHandler(MsgCSFrame* msgData, const SvcNetMatch& svcNetMatch, PlayerInfo* playerInfo);
	long long _flyEffectFrameHandler(MsgCSFlyEffectFrame* msgData, const SvcNetMatch& svcNetMatch, PlayerInfo* playerInfo);
	void _flyEffectCalculateFromSchedule30Handler();
	bool _flyEffectCalculateSingle(FlyEffectInfo * flyEffectInfo);

private:
	GameMap _gameMap;
	Framework::Plane _ground;
};
