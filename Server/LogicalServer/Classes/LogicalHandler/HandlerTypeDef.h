#pragma once
#include "../LogicalBase/Local.h"
#include "../LogicalBase/TypeDefine.h"


//���ٴ���������
enum QuickHandlerType
{
	QUICK_H_CLIENT_MSG_ARRIVED = 0,				//�ͻ�����Ϣ����
	QUICK_H_CLIENT_SOCKET_CLOSE,				//�ͻ����׽��ֶϿ�
	QUICK_H_SCHEDULE_30,						//���붨ʱ������
};
