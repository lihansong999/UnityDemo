using System.Collections.Generic;
using UnityEngine;
using UnityEngine.EventSystems;
using UnityEngine.Events;

public static class GameExtension
{
    public const float UIDesignWidth = 1280f;
    public const float UIDesignHeight = 720f;

    public static void DestroyAllChildren(this Transform parent)
    {
        for (int i = parent.childCount - 1; i >= 0; i--)
        {
            GameObject.Destroy(parent.GetChild(i).gameObject);
        }
    }

    public static T GetOrAddComponent<T>(this GameObject go) where T : Component
    {
        T component = go.GetComponent<T>();
        if (component == null)
            component = go.AddComponent<T>();
        return component;
    }

    public static Transform FindDepth(this Transform parent, string name)
    {
        foreach (Transform child in parent)
        {
            if (child.name == name)
                return child;
            var result = FindDepth(child, name);
            if (result != null)
                return result;
        }
        return null;
    }

    public static Transform FindBreadth(this Transform root, string name)
    {
        Queue<Transform> temp = new Queue<Transform>();
        temp.Enqueue(root);
        while (temp.Count > 0)
        {
            var trans = temp.Dequeue();
            if (string.Equals(trans.gameObject.name, name))
            {
                return trans;
            }
            for (int i = 0, length = trans.childCount; i < length; i++)
            {
                temp.Enqueue(trans.GetChild(i));
            }
        }
        return null;
    }

    public static IEnumerable<GameObject> ErgodicChild(this Transform trans)
    {
        for (int i = 0, length = trans.childCount; i < length; i++)
        {
            Transform child = trans.GetChild(i);
            yield return child.gameObject;
            foreach (var item in child.ErgodicChild())
            {
                yield return item.gameObject;
            }
        }
    }

    public static void RegisterTrigger(this EventTrigger eventTrigger, EventTriggerType triggerType, UnityAction<BaseEventData> callback)
    {
        EventTrigger.Entry entry = eventTrigger.triggers.Find((ite) => ite.eventID == triggerType);
        if (entry == null)
        {
            entry = new EventTrigger.Entry() { eventID = triggerType };
            eventTrigger.triggers.Add(entry);
        }
        entry.callback.AddListener(callback);
    }

    public static void CancelTrigger(this EventTrigger eventTrigger, EventTriggerType triggerType, UnityAction<BaseEventData> callback)
    {
        EventTrigger.Entry entry = eventTrigger.triggers.Find((ite) => ite.eventID == triggerType);
        if (entry != null)
        {
            entry.callback.RemoveListener(callback);
        }
    }

    public static float GetTouchCoefficient()
    {
        return 2.54f / Screen.dpi; // 目前采用"每厘米像素数量的倒数"作为系数
    }

    public static float GetUIScaleValueMatchShrink()
    {
        // 模拟 ScreenMatchMode.Shrink 模式的缩放计算
        float ratio1 = GameExtension.UIDesignWidth / GameExtension.UIDesignHeight;
        float ratio2 = Screen.width / (float)Screen.height;
        if (ratio2 >= ratio1)
        {
            return Screen.width / GameExtension.UIDesignWidth;
        }
        else
        {
            return Screen.height / GameExtension.UIDesignHeight;
        }
    }

    // Canvas是ScaleMode.ConstantPixelSize模式的UI需要进行模拟ScreenMatchMode.Shrink缩放
    public static void UIScaleMatchShrink(this RectTransform trans, ref float lastScale)
    {
        float newScale = GameExtension.GetUIScaleValueMatchShrink();

        if (newScale <= float.Epsilon || lastScale <= float.Epsilon)
            return;

        if (Mathf.Abs(newScale - lastScale) > float.Epsilon)
        {
            float scale = newScale / lastScale;
            Vector3 originScale = trans.localScale;
            trans.localScale = new Vector3(originScale.x * scale, originScale.y * scale, originScale.z * scale);
            lastScale = scale;
        }
    }

    public static float GetUIScaleValueMatchExpand()
    {
        // 模拟 ScreenMatchMode.Expand 模式的缩放计算
        float ratio1 = GameExtension.UIDesignWidth / GameExtension.UIDesignHeight;
        float ratio2 = Screen.width / (float)Screen.height;
        if (ratio2 >= ratio1)
        {
            return Screen.height / GameExtension.UIDesignHeight;
        }
        else
        {
            return Screen.width / GameExtension.UIDesignWidth;
        }
    }

    // Canvas是ScaleMode.ConstantPixelSize模式的UI需要进行模拟ScreenMatchMode.Expand缩放
    public static void UIScaleMatchExpand(this RectTransform trans, ref float lastScale)
    {
        float newScale = GameExtension.GetUIScaleValueMatchExpand();

        if (newScale <= float.Epsilon || lastScale <= float.Epsilon)
            return;

        if (Mathf.Abs(newScale - lastScale) > float.Epsilon)
        {
            float scale = newScale / lastScale;
            Vector3 originScale = trans.localScale;
            trans.localScale = new Vector3(originScale.x * scale, originScale.y * scale, originScale.z * scale);
            lastScale = scale;
        }
    }

    public static Vector2 TouchPointToLocalPointInUIRectangle(this RectTransform trans, Vector2 pos, Camera cam)
    {
        Vector2 inUIPos;
        if(RectTransformUtility.ScreenPointToLocalPointInRectangle(trans, pos, cam, out inUIPos))
        {
            return inUIPos;
        }
        return Vector2.positiveInfinity;
    }

    public static bool IsNameForPlaying(this Animator animator, int layer, string name, bool transition)
    {
        var info = animator.GetCurrentAnimatorStateInfo(layer);
        if (info.IsName(name))
        {
            return true;
        }
        if (transition && animator.IsInTransition(layer))
        {
            return animator.GetNextAnimatorStateInfo(0).IsName(name);
        }
        return false;
    }
}