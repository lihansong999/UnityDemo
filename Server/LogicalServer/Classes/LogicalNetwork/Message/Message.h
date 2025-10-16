#ifndef __MESSAGE_H__
#define __MESSAGE_H__

#include "../NetworkTools.h"


#pragma pack(push)
#pragma pack(1)

#define MSG_TYPE_HEARTBEAT		1	// 心跳（登录消息后才可发送）
#define	MSG_TYPE_LOGIN			2	// 登录
#define	MSG_TYPE_LOGOUT			3	// 登出
#define	MSG_TYPE_SYNCTIME		4	// 同步时间
#define	MSG_TYPE_SELFADDGAME	5	// 自己加入游戏
#define	MSG_TYPE_PLAYER_FRAME	6	// 玩家帧数据
#define	MSG_TYPE_FLY_EF_FRAME	7	// 飞行特效帧数据


#define MGS_FUN_F2I(a)	((int)(a * 1000.0f))
#define MGS_FUN_I2F(a)	((float)a / 1000.0f)


struct MsgHead
{
	unsigned char type;		// 消息类型
};

// 心跳
struct MsgCSHeartbeat
{
	MsgHead head;
};

struct MsgSCHeartbeat
{
	MsgSCHeartbeat() { head.type = MSG_TYPE_HEARTBEAT; }
	MsgHead head;
};

// 登录
struct MsgCSLogin
{
	MsgHead head;
};

struct MsgSCLogin
{
	MsgSCLogin() { head.type = MSG_TYPE_LOGIN; }
	MsgHead head;
	EntityID_t	selfEntityID;
};

// 登出
struct MsgCSLogout
{
	MsgHead head;
};

struct MsgSCLogout
{
	MsgSCLogout() { head.type = MSG_TYPE_LOGOUT; }
	MsgHead head;
	EntityID_t outEntityID; // 登出的玩家ID
};

// 同步时间
struct MsgCSSyncTime
{
	MsgHead head;
	long long reqTime;
};

struct MsgSCSyncTime
{
	MsgSCSyncTime() { head.type = MSG_TYPE_SYNCTIME; }
	MsgHead head;
	long long reqTime;
	long long serverTime;
};

// 加入游戏
struct MsgCSAddGame
{
	MsgHead head;
};

struct MsgSCAddGame
{
	MsgSCAddGame() { head.type = MSG_TYPE_SELFADDGAME; }
	MsgHead head;
	int birthPoint;	// 随机出生点
};

// 玩家帧
struct NetRoleFrame
{
	long long curTime = 0LL;        // 当前帧时间（毫秒）
	unsigned char infoType;         // 帧信息类型（enum FrameInfoType）
	unsigned char animMoveState;    // 动画Move状态（0待机，1走路，2跑步）
	int positionX;                  // 当前坐标位置
	int positionY;
	int positionZ;
	int rotationX;                  // 当前旋转
	int rotationY;
	int horizontalVelocityX;        // 当前水平速度（预测）
	int horizontalVelocityZ;
	unsigned char isJumping;        // 是否跳跃中（跳跃中不同步坐标）
};

struct MsgCSFrame
{
	MsgHead head;
	NetRoleFrame frameInfo;
};

struct MsgSCFrame
{
	MsgSCFrame() { head.type = MSG_TYPE_PLAYER_FRAME; }
	MsgHead head;
	EntityID_t frameEntityID;
	NetRoleFrame frameInfo;
};

// 飞行特效帧（全由服务器计算）
struct NetFlyEffectFrame
{
	long long curTime = 0LL;        // 当前帧时间（毫秒），与角色帧不同，由服务器计算会超前计算，时间会超过当前时间
	unsigned char infoType;         // 帧信息类型（1发射，2命中（爆炸），3消失）
	int positionX;                  // 当前坐标位置
	int positionY;
	int positionZ;
	int velocityX;					// 速度（每秒）
	int velocityY;
	int velocityZ;
};

struct MsgCSFlyEffectFrame // 客户端只在放技能时发送一次消息（Launch）
{
	MsgHead head;
	long long curTime = 0LL;        // 发射时间
	int positionX;                  // 发射坐标位置
	int positionY;
	int positionZ;
	int velocityX;					// 速度（每秒）
	int velocityY;
	int velocityZ;
};

struct MsgSCFlyEffectFrame
{
	MsgSCFlyEffectFrame() { head.type = MSG_TYPE_FLY_EF_FRAME; }
	MsgHead head;
	EntityID_t frameEntityID;
	NetFlyEffectFrame frameInfo;
};

#pragma pack(pop)

// 新加消息类型必须在此加入大小
extern bool CSMsgSizeMatch(const char* msgData, int msgSize);


#endif //!__MESSAGE_H__