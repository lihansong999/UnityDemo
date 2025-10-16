using JetBrains.Annotations;
using System.Collections;
using System.Collections.Generic;
using TMPro;
using UnityEngine;
using UnityEngine.UI;

public class GlobalValueManager : MonoBehaviour
{
    public float mouseSensitivity { get; private set; } = 100f;

    [SerializeField] private Slider _mouseSensitivitySlider;
    [SerializeField] private TextMeshProUGUI _mouseSensitivityText;

    static public GlobalValueManager instance { get; private set; }
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
    }
    private void OnDestroy()
    {
        if (this == instance)
        {
            instance = null;
        }

        _mouseSensitivitySlider.onValueChanged.RemoveListener(OnMouseSensitivitySliderChanged);
    }

    public const float WORLD_GRAVITY = -9.81f; // 世界重力
    public const float ROLE_JUMP_HEIGHT = 0.8f; // 角色跳跃高度
    public const float ROLE_WALK_SPEED = 3.5f; // 角色走路速度
    public const float ROLE_RUN_SPEED = 10f; // 角色跑步速度

    public const float MAX_CONSECUTIVE_FRAME_INTERVAL = 0.08f;   // 最大连续帧间隔时间

    private void Start()
    {
        _mouseSensitivitySlider.onValueChanged.AddListener(OnMouseSensitivitySliderChanged);
        _mouseSensitivitySlider.value = mouseSensitivity;
        _mouseSensitivityText.text = ((int)mouseSensitivity).ToString();
    }

    void OnMouseSensitivitySliderChanged(float value)
    {
        mouseSensitivity = value;
        _mouseSensitivityText.text = ((int)value).ToString();
    }
}
