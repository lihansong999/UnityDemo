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
            Destroy(gameObject); // ����������Ĺ������
            return;
        }
        instance = this;
        // DontDestroyOnLoad(gameObject); // �糡������
    }
    private void OnDestroy()
    {
        if (this == instance)
        {
            instance = null;
        }

        _mouseSensitivitySlider.onValueChanged.RemoveListener(OnMouseSensitivitySliderChanged);
    }

    public const float WORLD_GRAVITY = -9.81f; // ��������
    public const float ROLE_JUMP_HEIGHT = 0.8f; // ��ɫ��Ծ�߶�
    public const float ROLE_WALK_SPEED = 3.5f; // ��ɫ��·�ٶ�
    public const float ROLE_RUN_SPEED = 10f; // ��ɫ�ܲ��ٶ�

    public const float MAX_CONSECUTIVE_FRAME_INTERVAL = 0.08f;   // �������֡���ʱ��

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
