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

    // byte[] → Struct
    public static T BytesToStruct<T>(byte[] arr) where T : struct
    {
        int size = Marshal.SizeOf(typeof(T));
        if (arr.Length < size)
            throw new ArgumentException("字节长度不够还原结构体");

        IntPtr ptr = Marshal.AllocHGlobal(size);
        Marshal.Copy(arr, 0, ptr, size);
        T str = Marshal.PtrToStructure<T>(ptr);
        Marshal.FreeHGlobal(ptr);
        return str;
    }
}

public enum GameMsgType
{
    MSG_TYPE_HEARTBEAT = 1,	// 心跳（登录消息后才可发送）
    MSG_TYPE_LOGIN = 2,	    // 登录
    MSG_TYPE_LOGOUT = 3,	// 登出
    MSG_TYPE_SYNCTIME = 4,	// 同步时间
    MSG_TYPE_SELFADDGAME = 5,	// 自己加入游戏
    MSG_TYPE_PLAYER_FRAME = 6,	// 玩家帧数据
    MSG_TYPE_FLY_EF_FRAME =	7,	// 飞行特效帧数据
}

//
// 公共头
//
[StructLayout(LayoutKind.Sequential, Pack = 1)]
public struct MsgHead
{
    public byte type;   // 消息类型
}

//
// 心跳
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
// 登录
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
// 登出
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
// 同步时间
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
    public long reqTime;       // long long -> long (C# 64位)
    public long serverTime;
}

//
// 加入游戏
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
// 玩家帧数据
//
public enum FrameInfoType
{
    None = 0,
    PosRotate,      // 同步位置和旋转帧数据
    JumpStart,      // 起跳
    AttackStart,    // 发起攻击
}

[StructLayout(LayoutKind.Sequential, Pack = 1)]
public struct NetRoleFrame
{
    public long curTime;                   // 当前帧时间（毫秒）
    public byte infoType;                  // 帧信息类型（enum FrameInfoType）
    public byte animMoveState;             // 动画Move状态（0待机，1走路，2跑步）
    public int positionX;                  // 当前坐标位置
    public int positionY;
    public int positionZ;
    public int rotationX;                  // 当前旋转
    public int rotationY;
    public int horizontalVelocityX;        // 当前水平速度（预测）
    public int horizontalVelocityZ;
    public byte isJumping;                 // 是否跳跃中（跳跃中并且非起跳帧时不同步坐标，别人的跳跃过程由本地模拟）
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

// 飞行特效帧（全由服务器计算）
[StructLayout(LayoutKind.Sequential, Pack = 1)]
public struct NetFlyEffectFrame
{
    public long curTime;        // 当前帧时间（毫秒），与角色帧不同，由服务器计算会超前计算，时间会超过当前时间
    public byte infoType;       // 帧信息类型（1发射，2命中（爆炸），3消失）
    public int positionX;       // 当前坐标位置
    public int positionY;
    public int positionZ;
    public int velocityX;       // 速度（每秒）
    public int velocityY;
    public int velocityZ;
};

[StructLayout(LayoutKind.Sequential, Pack = 1)]
public struct MsgCSFlyEffectFrame // 客户端只在放技能时发送一次消息（Launch）
{
    public MsgHead head;
    public long curTime;        // 发射时间
	public int positionX;       // 发射坐标位置
    public int positionY;
    public int positionZ;
    public int velocityX;       // 速度（每秒）
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
