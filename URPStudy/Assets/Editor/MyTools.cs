using System.Collections;
using System.Collections.Generic;
using System.IO;
using System.Text;
using UnityEditor;
using UnityEditor.Timeline.Actions;
using UnityEngine;

public class MyTools : MonoBehaviour
{
    [MenuItem("�ҵĹ���/��ʼ����Ϸ &i")]
    static void InitGame()
    {
        GameManager.instance.InitGame();
    }

    [MenuItem("�ҵĹ���/�Լ�������Ϸ�������� &o")]
    static void SelfAddGame()
    {
        //GameManager.instance.AddSelfPlayerToGameForBirth(Random.Range(0, 7));
    }

    [MenuItem("�ҵĹ���/���뽩ʬ &j")]
    static void ZombieAddGame()
    {
        GameManager.instance.AddZombieToGame();
    }

    [MenuItem("�ҵĹ���/ɾ����ʬ &k")]
    static void ZombieRemoveGame()
    {
        GameManager.instance.RemoveZombieFromGame();
    }

    [MenuItem("�ҵĹ���/������ײ��")]
    public static void ExportAABBs()
    {
        // �ҵ������еĽڵ�
        GameObject root = GameObject.Find("GameMap1/CollisionNode");
        if (root == null)
        {
            Debug.LogError("û�ҵ��ڵ� GameMap1/CollisionNode");
            return;
        }

        var boxColliders = root.GetComponentsInChildren<BoxCollider>(true);
        if (boxColliders.Length == 0)
        {
            Debug.LogWarning("û���ҵ� BoxCollider");
            return;
        }

        StringBuilder sb = new StringBuilder();

        string FormatFloat(float value)
        {
            // ȷ��������� "11.0f" �� "-3.25f"
            string s = value.ToString("0.######", System.Globalization.CultureInfo.InvariantCulture);
            if (!s.Contains(".")) s += ".0";
            return s + "f";
        }

        foreach (var box in boxColliders)
        {
            Bounds b = box.bounds;
            Vector3 min = b.min;
            Vector3 max = b.max;

            //sb.AppendLine($"_gameMapBuildingCollisionAABBs.push_back(AABB(Vec3({min.x}f, {min.y}f, {min.z}f), Vec3({max.x}f, {max.y}f, {max.z}f)));");
            sb.AppendLine($"_gameMapBuildingCollisionAABBs.push_back(AABB(" + $"Vec3({FormatFloat(min.x)}, {FormatFloat(min.y)}, {FormatFloat(min.z)}), " + $"Vec3({FormatFloat(max.x)}, {FormatFloat(max.y)}, {FormatFloat(max.z)})));");
        }


        // ��������Ŀ���
        string path = Path.Combine(Application.dataPath, "../ExportedCollisionAABB.cpp");
        File.WriteAllText(path, sb.ToString(), Encoding.UTF8);

        Debug.Log($"�ɹ����� {boxColliders.Length} �� BoxCollider ��: {path}");
        EditorUtility.RevealInFinder(path);
    }
}
