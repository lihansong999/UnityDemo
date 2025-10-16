using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public enum GameEventKey
{
    GameCameraModeChanged,
}

public class GameEventManager : MonoBehaviour
{
    static public GameEventManager instance { get; private set; }
    private void Awake()
    {
        if (instance != null && instance != this)
        {
            Debug.LogError("repeat create manage object name: " + this.GetType());
            Destroy(gameObject);
            return;
        }
        instance = this;
        // DontDestroyOnLoad(gameObject); // 跨场景保留
    }
    private void OnDestroy()
    {
        if (this == instance)
        {
            instance = null;
        }
    }

    private Dictionary<GameEventKey, Action<GameEventParam> > _eventTable = new Dictionary<GameEventKey, Action<GameEventParam> >();

    public void Register(GameEventKey k, Action<GameEventParam> callback)
    {
        if (!_eventTable.ContainsKey(k))
            _eventTable[k] = null;
        _eventTable[k] += callback;
    }

    public void Unregister(GameEventKey k, Action<GameEventParam> callback)
    {
        if (_eventTable.ContainsKey(k))
            _eventTable[k] -= callback;
    }

    public void Trigger(GameEventKey k, GameEventParam param)
    {
        if (_eventTable.ContainsKey(k))
            _eventTable[k]?.Invoke(param);
    }
}

// 事件参数基类
public class GameEventParam { };

// 相机切换完成的事件参数
public class GameCameraModeChangedEventParam : GameEventParam
{
    public GameCameraModeChangedEventParam(GameCameraMode f, GameCameraMode t) { from = f; to = t; }
    public GameCameraMode from { get; private set; }
    public GameCameraMode to { get; private set; }
}