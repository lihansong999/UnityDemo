using System.Collections;
using System.Collections.Generic;
using TMPro;
using UnityEngine;
using UnityEngine.UI;

public class AirBox : MonoBehaviour
{
    static List<GameObject> s_CurShowAirBox = new List<GameObject>();

    [Range(1.0f, 10.0f)]
    public float m_DisplayDuration = 3.5f;

    float m_RemainingTime = 0.0f;

    RectTransform m_RectTransform = null;

    bool m_UpMoveAllCalled = false;

    // Start is called before the first frame update
    void Start()
    {
        m_RemainingTime = m_DisplayDuration;

        m_RectTransform = this.gameObject.GetComponent<RectTransform>();
    }

    // Update is called once per frame
    void Update()
    {
        if (m_RemainingTime > 0)
        {
            m_RemainingTime -= Time.deltaTime;
        }

        if(m_RemainingTime <= 0)
        {
            Destroy(this.gameObject);
        }
    }

    private void OnRectTransformDimensionsChange()
    {
        if (!m_UpMoveAllCalled && m_RectTransform.rect.height > float.Epsilon)
        {
            m_UpMoveAllCalled = true;

            UpMoveAll(m_RectTransform.rect.height);

            s_CurShowAirBox.Add(this.gameObject);
        }
    }

    void OnDestroy()
    {
        s_CurShowAirBox.Remove(this.gameObject);
    }

    static private void UpMoveAll(float distance)
    {
        foreach (GameObject obj in s_CurShowAirBox)
        {
            RectTransform rt = obj.GetComponent<RectTransform>();
            rt.anchoredPosition = new Vector2(rt.anchoredPosition.x, rt.anchoredPosition.y + distance + 5.0f);
        }
    }

    static public void Show(string text)
    {
        GameObject systemCanvas1Panel = GameObject.Find("UI/Canvas/SystemUI");
        if (systemCanvas1Panel)
        {
            GameObject airBoxPrefab = Resources.Load<GameObject>("Prefab/Common/AirBox");

            if (airBoxPrefab)
            {
                GameObject airBox = Instantiate(airBoxPrefab, systemCanvas1Panel.transform);
                if (airBox)
                {
                    airBox.transform.Find("Text").GetComponent<TextMeshProUGUI>().text = text;
                }
            }
        }
    }
}
