#include "stdafx.h"
#include "Message.h"


bool CSMsgSizeMatch(const char* msgData, int msgSize)
{
	if (msgSize >= sizeof(MsgHead))
	{
		switch (((MsgHead*)msgData)->type)
		{
		case MSG_TYPE_HEARTBEAT:
			return sizeof(MsgCSHeartbeat) == msgSize;
		case MSG_TYPE_LOGIN:
			return sizeof(MsgCSLogin) == msgSize;
		case MSG_TYPE_LOGOUT:
			return sizeof(MsgCSLogout) == msgSize;
		case MSG_TYPE_SYNCTIME:
			return sizeof(MsgCSSyncTime) == msgSize;
		case MSG_TYPE_SELFADDGAME:
			return sizeof(MsgCSAddGame) == msgSize;
		case MSG_TYPE_PLAYER_FRAME:
			return sizeof(MsgCSFrame) == msgSize;
		case MSG_TYPE_FLY_EF_FRAME:
			return sizeof(MsgCSFlyEffectFrame) == msgSize;
		}
	}
	return false;
}