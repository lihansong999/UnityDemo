using System.Collections;
using System.Collections.Generic;
using Unity.VisualScripting;
using UnityEngine;

public enum GameStatus
{
    Watching,       // 观战中
    Fighting,       // 参与战斗中
}

public class GameManager : MonoBehaviour
{
    static public GameManager instance { get; private set; }
    private void Awake()
    {
        if (instance != null && instance != this)
        {
            Debug.LogError("repeat create manage object name: " + this.GetType());
            Destroy(gameObject); // 不创建多余的管理对象
            return;
        }
        instance = this;
        // DontDestroyOnLoad(gameObject); // 跨场景保留

        Application.runInBackground = true;
        QualitySettings.vSyncCount = 0;
        switch (Application.platform)
        {
            case RuntimePlatform.Android:
            case RuntimePlatform.IPhonePlayer:
            case RuntimePlatform.WebGLPlayer:   // WebGL可能无效，需要在 JS requestAnimationFrame 或游戏逻辑层做限制
                Application.targetFrameRate = 60;
                break;
            default: // PC / Mac
                var refreshRateRatio = Screen.currentResolution.refreshRateRatio;
                int refreshRate = Mathf.RoundToInt((float)refreshRateRatio.numerator / refreshRateRatio.denominator);
                if (refreshRate > 144)
                    Application.targetFrameRate = 144;
                else if (refreshRate > 60)
                    Application.targetFrameRate = refreshRate;
                else
                    Application.targetFrameRate = 60;
                break;
        }
    }
    private void OnDestroy()
    {
        if (this == instance)
        {
            instance = null;
        }
    }

    private void Start()
    {
        _dynamicElementNodeInGameMap1 = _gameMap1.transform.Find("DynamicElementNode").gameObject;

        InitGame();
    }

    private void Update()
    {
        if (NetworkManager.instance.isGameing && Input.GetKeyUp(KeyCode.T))
        {
            UIManager.instance.SwitchSettingUI(!UIManager.instance.IsShowingSettingUI());
        }

        UpdateSwitchCursor();
    }

    //////////////////////////////////////////// 鼠标相关 ////////////////////////////////////////////

    // 更新鼠标开关
    private void UpdateSwitchCursor()
    {
        if (IsShowingUIForGameOperation())
        {
            Cursor.lockState = CursorLockMode.None;
            Cursor.visible = true;
        }
        else
        {
            Cursor.lockState = CursorLockMode.Locked;
            Cursor.visible = false;
        }
    }

    //////////////////////////////////////////// 界面相关 ////////////////////////////////////////////
    
    // 是否有会停止游戏操作的UI正在显示
    public bool IsShowingUIForGameOperation()
    {
        return UIManager.instance.IsShowingSettingUI() || UIManager.instance.IsShowingNetWorkUI();
    }

    //////////////////////////////////////////// 游戏逻辑相关 ////////////////////////////////////////////

    public GameStatus curGameStatus { get; private set; } = GameStatus.Watching; // 游戏状态
    [SerializeField] private GameObject _gameMap1;      // 游戏地图
    private GameObject _dynamicElementNodeInGameMap1;   // 地图中的动态元素根节点（玩家，怪物，子弹...）
    private GameObject _mainRole;   // 自己的角色对象
    private Dictionary<long, RoleBehaviour> _roleDictionary = new Dictionary<long, RoleBehaviour>();    // 游戏中角色对象脚本实例
    private Dictionary<long, FlyEffectBehaviour> _flyEffectDictionary = new Dictionary<long, FlyEffectBehaviour>();    // 游戏中飞行特效对象脚本实例
    [HideInInspector] public long selfEntityID;             // 当前登录的自己玩家ID，登录返回消息中下发

    // 初始化游戏，可复用
    public void InitGame()
    {
        GameCameraManager.instance.ResetCamera();
        if (_mainRole != null)
        {
            GameCameraManager.instance.MoveCameraFTrackTarget(_mainRole);
        }
        _dynamicElementNodeInGameMap1.transform.DestroyAllChildren();
        _mainRole = null;
        curGameStatus = GameStatus.Watching;
        _roleDictionary.Clear();
        _flyEffectDictionary.Clear();
        selfEntityID = 0;
    }

    // 加入自己到游戏中，出生的时候，会先尝试删除以前的自己
    public void AddSelfPlayerToGameForBirth(int birthPoint)
    {
        if (_mainRole != null)
        {
            Destroy(_mainRole);
            _mainRole = null;
        }
        GameObject mainRolePrefab = Resources.Load<GameObject>("Prefab/Role/MainRole/MainRole");
        _mainRole = Instantiate(mainRolePrefab, _dynamicElementNodeInGameMap1.transform, false);
        _mainRole.GetComponent<RoleBehaviour>()._entityID = GameManager.instance.selfEntityID;
        Vector3 basePos = _gameMap1.transform.Find("PlayerBirthPoint" + birthPoint).position;
        _mainRole.transform.position = new Vector3(basePos.x + Random.Range(-2.5f, 2.5f), basePos.y, basePos.z + Random.Range(-2.5f, 2.5f));
        GameCameraManager.instance.SetCamera13TrackTarget(_mainRole);
        GameCameraManager.instance.SetCurCameraMode(GameCameraMode.Third);
        curGameStatus = GameStatus.Fighting;
    }

    // 帧消息
    public void FrameComing(long entityID, in NetRoleFrame frameInfo)
    {
        RoleBehaviour roleBehaviourCom;
        if (!_roleDictionary.TryGetValue(entityID, out roleBehaviourCom))
        {
            // 加入其他玩家
            GameObject netMainRolePrefab = Resources.Load<GameObject>("Prefab/Role/MainRole/NetMainRole");
            var netMainRole = Instantiate(netMainRolePrefab, _dynamicElementNodeInGameMap1.transform, false);
            netMainRole.transform.position = new Vector3(NetWorkUtils.i2f(frameInfo.positionX), NetWorkUtils.i2f(frameInfo.positionY), NetWorkUtils.i2f(frameInfo.positionZ));
            roleBehaviourCom = netMainRole.GetComponent<RoleBehaviour>();
            roleBehaviourCom._entityID = entityID;
            _roleDictionary.TryAdd(entityID, roleBehaviourCom);
        }
        NetRoleBehaviour netRoleBehaviourCom = roleBehaviourCom as NetRoleBehaviour;
        netRoleBehaviourCom.AddNetFrame(frameInfo);
    }

    // 法术帧消息（飞行特效）
    public void MagicFrameComing(in MsgSCFlyEffectFrame frameInfo)
    {
        FlyEffectBehaviour flyEffectBehaviourCom;
        if (!_flyEffectDictionary.TryGetValue(frameInfo.frameEntityID, out flyEffectBehaviourCom))
        {
            if(frameInfo.frameInfo.infoType != 1) {
                return;
            }
            // 加入飞行特效
            GameObject netMagicFlyEffectPrefab = Resources.Load<GameObject>("Prefab/Effect/NetMagicFlyEffect");
            var netMagicFlyEffect = Instantiate(netMagicFlyEffectPrefab, _dynamicElementNodeInGameMap1.transform, false);
            netMagicFlyEffect.transform.position = new Vector3(NetWorkUtils.i2f(frameInfo.frameInfo.positionX), NetWorkUtils.i2f(frameInfo.frameInfo.positionY), NetWorkUtils.i2f(frameInfo.frameInfo.positionZ));
            flyEffectBehaviourCom = netMagicFlyEffect.GetComponent<FlyEffectBehaviour>();
            flyEffectBehaviourCom._entityID = frameInfo.frameEntityID;
            _flyEffectDictionary.TryAdd(frameInfo.frameEntityID, flyEffectBehaviourCom);
        }
        NetMagicFlyEffectBehaviour netMagicFlyEffectBehaviourCom = flyEffectBehaviourCom as NetMagicFlyEffectBehaviour;
        netMagicFlyEffectBehaviourCom.AddNetFrame(frameInfo);
    }

    // 播放技能命中爆炸特效
    public void PlayMagicBoom(Vector3 pos)
    {
        GameObject magicBoomEffectPrefab = Resources.Load<GameObject>("Prefab/Effect/MagicBoomEffect");
        var magicBoomEffect = Instantiate(magicBoomEffectPrefab, _dynamicElementNodeInGameMap1.transform, false);
        magicBoomEffect.transform.position = pos;
    }

    public void DestroyMagic(long entityID)
    {
        FlyEffectBehaviour flyEffectBehaviourCom;
        if (_flyEffectDictionary.TryGetValue(entityID, out flyEffectBehaviourCom))
        {
            _flyEffectDictionary.Remove(entityID);
            Destroy(flyEffectBehaviourCom.gameObject);
        }
    }

    // 删除玩家（可能是自己）
    public void RemovePlayerFromGame(long entityID)
    {
        if (selfEntityID == entityID)
        {
            AirBox.Show("您被踢出了游戏");
            InitGame();
        }
        else
        {
            RoleBehaviour roleBehaviourCom;
            if (_roleDictionary.TryGetValue(entityID, out roleBehaviourCom))
            {
                _roleDictionary.Remove(entityID);
                Destroy(roleBehaviourCom.gameObject);

                // 这里考虑加入观战时相机切换到下一个跟随
            }
        }
    }

    // 加入僵尸怪
    public void AddZombieToGame()
    {
        RoleBehaviour roleBehaviourCom;
        if (!_roleDictionary.TryGetValue(2005, out roleBehaviourCom))
        {
            GameObject zombieRolePrefab = Resources.Load<GameObject>("Prefab/Role/Zombie/NetZombieRole");
            var zombieRole = Instantiate(zombieRolePrefab, _dynamicElementNodeInGameMap1.transform, false);
            zombieRole.transform.position = _gameMap1.transform.Find("PlayerBirthPoint" + Random.Range(0, 7)).position;


            _roleDictionary.TryAdd(2005, zombieRole.GetComponent<RoleBehaviour>());
        }
    }

    // 删除僵尸怪
    public void RemoveZombieFromGame()
    {
        RoleBehaviour roleBehaviourCom;
        if (_roleDictionary.TryGetValue(2005, out roleBehaviourCom))
        {
            _roleDictionary.Remove(2005);
            Destroy(roleBehaviourCom.gameObject);

            // 这里注意观战时相机要切换到下一个跟随
        }
    }
}
