#pragma once
#include "../LogicalBase/Local.h"
#include "../LogicalBase/TypeDefine.h"


//快速处理器类型
enum QuickHandlerType
{
	QUICK_H_CLIENT_MSG_ARRIVED = 0,				//客户端消息到来
	QUICK_H_CLIENT_SOCKET_CLOSE,				//客户端套接字断开
	QUICK_H_SCHEDULE_30,						//毫秒定时器触发
};
