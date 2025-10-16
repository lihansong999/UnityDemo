#ifndef __MESSAGE_H__
#define __MESSAGE_H__

#include "../NetworkTools.h"


#pragma pack(push)
#pragma pack(1)

#define MSG_TYPE_HEARTBEAT		1	// ��������¼��Ϣ��ſɷ��ͣ�
#define	MSG_TYPE_LOGIN			2	// ��¼
#define	MSG_TYPE_LOGOUT			3	// �ǳ�
#define	MSG_TYPE_SYNCTIME		4	// ͬ��ʱ��
#define	MSG_TYPE_SELFADDGAME	5	// �Լ�������Ϸ
#define	MSG_TYPE_PLAYER_FRAME	6	// ���֡����
#define	MSG_TYPE_FLY_EF_FRAME	7	// ������Ч֡����


#define MGS_FUN_F2I(a)	((int)(a * 1000.0f))
#define MGS_FUN_I2F(a)	((float)a / 1000.0f)


struct MsgHead
{
	unsigned char type;		// ��Ϣ����
};

// ����
struct MsgCSHeartbeat
{
	MsgHead head;
};

struct MsgSCHeartbeat
{
	MsgSCHeartbeat() { head.type = MSG_TYPE_HEARTBEAT; }
	MsgHead head;
};

// ��¼
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

// �ǳ�
struct MsgCSLogout
{
	MsgHead head;
};

struct MsgSCLogout
{
	MsgSCLogout() { head.type = MSG_TYPE_LOGOUT; }
	MsgHead head;
	EntityID_t outEntityID; // �ǳ������ID
};

// ͬ��ʱ��
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

// ������Ϸ
struct MsgCSAddGame
{
	MsgHead head;
};

struct MsgSCAddGame
{
	MsgSCAddGame() { head.type = MSG_TYPE_SELFADDGAME; }
	MsgHead head;
	int birthPoint;	// ���������
};

// ���֡
struct NetRoleFrame
{
	long long curTime = 0LL;        // ��ǰ֡ʱ�䣨���룩
	unsigned char infoType;         // ֡��Ϣ���ͣ�enum FrameInfoType��
	unsigned char animMoveState;    // ����Move״̬��0������1��·��2�ܲ���
	int positionX;                  // ��ǰ����λ��
	int positionY;
	int positionZ;
	int rotationX;                  // ��ǰ��ת
	int rotationY;
	int horizontalVelocityX;        // ��ǰˮƽ�ٶȣ�Ԥ�⣩
	int horizontalVelocityZ;
	unsigned char isJumping;        // �Ƿ���Ծ�У���Ծ�в�ͬ�����꣩
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

// ������Ч֡��ȫ�ɷ��������㣩
struct NetFlyEffectFrame
{
	long long curTime = 0LL;        // ��ǰ֡ʱ�䣨���룩�����ɫ֡��ͬ���ɷ���������ᳬǰ���㣬ʱ��ᳬ����ǰʱ��
	unsigned char infoType;         // ֡��Ϣ���ͣ�1���䣬2���У���ը����3��ʧ��
	int positionX;                  // ��ǰ����λ��
	int positionY;
	int positionZ;
	int velocityX;					// �ٶȣ�ÿ�룩
	int velocityY;
	int velocityZ;
};

struct MsgCSFlyEffectFrame // �ͻ���ֻ�ڷż���ʱ����һ����Ϣ��Launch��
{
	MsgHead head;
	long long curTime = 0LL;        // ����ʱ��
	int positionX;                  // ��������λ��
	int positionY;
	int positionZ;
	int velocityX;					// �ٶȣ�ÿ�룩
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

// �¼���Ϣ���ͱ����ڴ˼����С
extern bool CSMsgSizeMatch(const char* msgData, int msgSize);


#endif //!__MESSAGE_H__