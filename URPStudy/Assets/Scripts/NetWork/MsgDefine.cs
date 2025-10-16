using UnityEngine;
using System;
using System.Runtime.InteropServices;

public static class NetWorkUtils
{
    public static int f2i(float a)
    {
        return (int)(a * 1000.0f);
    }

    public static float i2f(int a)
    {
        return (float)a / 1000.0f;
    }

    // byte[] �� Struct
    public static T BytesToStruct<T>(byte[] arr) where T : struct
    {
        int size = Marshal.SizeOf(typeof(T));
        if (arr.Length < size)
            throw new ArgumentException("�ֽڳ��Ȳ�����ԭ�ṹ��");

        IntPtr ptr = Marshal.AllocHGlobal(size);
        Marshal.Copy(arr, 0, ptr, size);
        T str = Marshal.PtrToStructure<T>(ptr);
        Marshal.FreeHGlobal(ptr);
        return str;
    }
}

public enum GameMsgType
{
    MSG_TYPE_HEARTBEAT = 1,	// ��������¼��Ϣ��ſɷ��ͣ�
    MSG_TYPE_LOGIN = 2,	    // ��¼
    MSG_TYPE_LOGOUT = 3,	// �ǳ�
    MSG_TYPE_SYNCTIME = 4,	// ͬ��ʱ��
    MSG_TYPE_SELFADDGAME = 5,	// �Լ�������Ϸ
    MSG_TYPE_PLAYER_FRAME = 6,	// ���֡����
    MSG_TYPE_FLY_EF_FRAME =	7,	// ������Ч֡����
}

//
// ����ͷ
//
[StructLayout(LayoutKind.Sequential, Pack = 1)]
public struct MsgHead
{
    public byte type;   // ��Ϣ����
}

//
// ����
//
[StructLayout(LayoutKind.Sequential, Pack = 1)]
public struct MsgCSHeartbeat
{
    public MsgHead head;
}

[StructLayout(LayoutKind.Sequential, Pack = 1)]
public struct MsgSCHeartbeat
{
    public MsgHead head;
}

//
// ��¼
//
[StructLayout(LayoutKind.Sequential, Pack = 1)]
public struct MsgCSLogin
{
    public MsgHead head;
}

[StructLayout(LayoutKind.Sequential, Pack = 1)]
public struct MsgSCLogin
{
    public MsgHead head;
    public long selfEntityID;
}

//
// �ǳ�
//
[StructLayout(LayoutKind.Sequential, Pack = 1)]
public struct MsgCSLogout
{
    public MsgHead head;
}

[StructLayout(LayoutKind.Sequential, Pack = 1)]
public struct MsgSCLogout
{
    public MsgHead head;
    public long outEntityID;
}

//
// ͬ��ʱ��
//
[StructLayout(LayoutKind.Sequential, Pack = 1)]
public struct MsgCSSyncTime
{
    public MsgHead head;
    public long reqTime;
}

[StructLayout(LayoutKind.Sequential, Pack = 1)]
public struct MsgSCSyncTime
{
    public MsgHead head;
    public long reqTime;       // long long -> long (C# 64λ)
    public long serverTime;
}

//
// ������Ϸ
//
[StructLayout(LayoutKind.Sequential, Pack = 1)]
public struct MsgCSAddGame
{
    public MsgHead head;
}

[StructLayout(LayoutKind.Sequential, Pack = 1)]
public struct MsgSCAddGame
{
    public MsgHead head;
    public int birthPoint;
}

//
// ���֡����
//
public enum FrameInfoType
{
    None = 0,
    PosRotate,      // ͬ��λ�ú���ת֡����
    JumpStart,      // ����
    AttackStart,    // ���𹥻�
}

[StructLayout(LayoutKind.Sequential, Pack = 1)]
public struct NetRoleFrame
{
    public long curTime;                   // ��ǰ֡ʱ�䣨���룩
    public byte infoType;                  // ֡��Ϣ���ͣ�enum FrameInfoType��
    public byte animMoveState;             // ����Move״̬��0������1��·��2�ܲ���
    public int positionX;                  // ��ǰ����λ��
    public int positionY;
    public int positionZ;
    public int rotationX;                  // ��ǰ��ת
    public int rotationY;
    public int horizontalVelocityX;        // ��ǰˮƽ�ٶȣ�Ԥ�⣩
    public int horizontalVelocityZ;
    public byte isJumping;                 // �Ƿ���Ծ�У���Ծ�в��ҷ�����֡ʱ��ͬ�����꣬���˵���Ծ�����ɱ���ģ�⣩
}

[StructLayout(LayoutKind.Sequential, Pack = 1)]
public struct MsgCSFrame
{
    public MsgHead head;
    public NetRoleFrame frameInfo;
}

[StructLayout(LayoutKind.Sequential, Pack = 1)]
public struct MsgSCFrame
{
    public MsgHead head;
    public long frameEntityID;
    public NetRoleFrame frameInfo;
}

// ������Ч֡��ȫ�ɷ��������㣩
[StructLayout(LayoutKind.Sequential, Pack = 1)]
public struct NetFlyEffectFrame
{
    public long curTime;        // ��ǰ֡ʱ�䣨���룩�����ɫ֡��ͬ���ɷ���������ᳬǰ���㣬ʱ��ᳬ����ǰʱ��
    public byte infoType;       // ֡��Ϣ���ͣ�1���䣬2���У���ը����3��ʧ��
    public int positionX;       // ��ǰ����λ��
    public int positionY;
    public int positionZ;
    public int velocityX;       // �ٶȣ�ÿ�룩
    public int velocityY;
    public int velocityZ;
};

[StructLayout(LayoutKind.Sequential, Pack = 1)]
public struct MsgCSFlyEffectFrame // �ͻ���ֻ�ڷż���ʱ����һ����Ϣ��Launch��
{
    public MsgHead head;
    public long curTime;        // ����ʱ��
	public int positionX;       // ��������λ��
    public int positionY;
    public int positionZ;
    public int velocityX;       // �ٶȣ�ÿ�룩
    public int velocityY;
    public int velocityZ;
};

[StructLayout(LayoutKind.Sequential, Pack = 1)]
public struct MsgSCFlyEffectFrame
{
    public MsgHead head;
    public long frameEntityID;
    public NetFlyEffectFrame frameInfo;
};
