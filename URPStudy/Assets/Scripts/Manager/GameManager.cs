using System.Collections;
using System.Collections.Generic;
using Unity.VisualScripting;
using UnityEngine;

public enum GameStatus
{
    Watching,       // ��ս��
    Fighting,       // ����ս����
}

public class GameManager : MonoBehaviour
{
    static public GameManager instance { get; private set; }
    private void Awake()
    {
        if (instance != null && instance != this)
        {
            Debug.LogError("repeat create manage object name: " + this.GetType());
            Destroy(gameObject); // ����������Ĺ������
            return;
        }
        instance = this;
        // DontDestroyOnLoad(gameObject); // �糡������

        Application.runInBackground = true;
        QualitySettings.vSyncCount = 0;
        switch (Application.platform)
        {
            case RuntimePlatform.Android:
            case RuntimePlatform.IPhonePlayer:
            case RuntimePlatform.WebGLPlayer:   // WebGL������Ч����Ҫ�� JS requestAnimationFrame ����Ϸ�߼���������
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

    //////////////////////////////////////////// ������ ////////////////////////////////////////////

    // ������꿪��
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

    //////////////////////////////////////////// ������� ////////////////////////////////////////////
    
    // �Ƿ��л�ֹͣ��Ϸ������UI������ʾ
    public bool IsShowingUIForGameOperation()
    {
        return UIManager.instance.IsShowingSettingUI() || UIManager.instance.IsShowingNetWorkUI();
    }

    //////////////////////////////////////////// ��Ϸ�߼���� ////////////////////////////////////////////

    public GameStatus curGameStatus { get; private set; } = GameStatus.Watching; // ��Ϸ״̬
    [SerializeField] private GameObject _gameMap1;      // ��Ϸ��ͼ
    private GameObject _dynamicElementNodeInGameMap1;   // ��ͼ�еĶ�̬Ԫ�ظ��ڵ㣨��ң�����ӵ�...��
    private GameObject _mainRole;   // �Լ��Ľ�ɫ����
    private Dictionary<long, RoleBehaviour> _roleDictionary = new Dictionary<long, RoleBehaviour>();    // ��Ϸ�н�ɫ����ű�ʵ��
    private Dictionary<long, FlyEffectBehaviour> _flyEffectDictionary = new Dictionary<long, FlyEffectBehaviour>();    // ��Ϸ�з�����Ч����ű�ʵ��
    [HideInInspector] public long selfEntityID;             // ��ǰ��¼���Լ����ID����¼������Ϣ���·�

    // ��ʼ����Ϸ���ɸ���
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

    // �����Լ�����Ϸ�У�������ʱ�򣬻��ȳ���ɾ����ǰ���Լ�
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

    // ֡��Ϣ
    public void FrameComing(long entityID, in NetRoleFrame frameInfo)
    {
        RoleBehaviour roleBehaviourCom;
        if (!_roleDictionary.TryGetValue(entityID, out roleBehaviourCom))
        {
            // �����������
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

    // ����֡��Ϣ��������Ч��
    public void MagicFrameComing(in MsgSCFlyEffectFrame frameInfo)
    {
        FlyEffectBehaviour flyEffectBehaviourCom;
        if (!_flyEffectDictionary.TryGetValue(frameInfo.frameEntityID, out flyEffectBehaviourCom))
        {
            if(frameInfo.frameInfo.infoType != 1) {
                return;
            }
            // ���������Ч
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

    // ���ż������б�ը��Ч
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

    // ɾ����ң��������Լ���
    public void RemovePlayerFromGame(long entityID)
    {
        if (selfEntityID == entityID)
        {
            AirBox.Show("�����߳�����Ϸ");
            InitGame();
        }
        else
        {
            RoleBehaviour roleBehaviourCom;
            if (_roleDictionary.TryGetValue(entityID, out roleBehaviourCom))
            {
                _roleDictionary.Remove(entityID);
                Destroy(roleBehaviourCom.gameObject);

                // ���￼�Ǽ����սʱ����л�����һ������
            }
        }
    }

    // ���뽩ʬ��
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

    // ɾ����ʬ��
    public void RemoveZombieFromGame()
    {
        RoleBehaviour roleBehaviourCom;
        if (_roleDictionary.TryGetValue(2005, out roleBehaviourCom))
        {
            _roleDictionary.Remove(2005);
            Destroy(roleBehaviourCom.gameObject);

            // ����ע���սʱ���Ҫ�л�����һ������
        }
    }
}
