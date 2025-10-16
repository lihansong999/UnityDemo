using System.Collections;
using System.Collections.Generic;
using System.IO;
using System.Text;
using UnityEditor;
using UnityEditor.Timeline.Actions;
using UnityEngine;

public class MyTools : MonoBehaviour
{
    [MenuItem("我的工具/初始化游戏 &i")]
    static void InitGame()
    {
        GameManager.instance.InitGame();
    }

    [MenuItem("我的工具/自己加入游戏（出生） &o")]
    static void SelfAddGame()
    {
        //GameManager.instance.AddSelfPlayerToGameForBirth(Random.Range(0, 7));
    }

    [MenuItem("我的工具/加入僵尸 &j")]
    static void ZombieAddGame()
    {
        GameManager.instance.AddZombieToGame();
    }

    [MenuItem("我的工具/删除僵尸 &k")]
    static void ZombieRemoveGame()
    {
        GameManager.instance.RemoveZombieFromGame();
    }

    [MenuItem("我的工具/导出碰撞体")]
    public static void ExportAABBs()
    {
        // 找到场景中的节点
        GameObject root = GameObject.Find("GameMap1/CollisionNode");
        if (root == null)
        {
            Debug.LogError("没找到节点 GameMap1/CollisionNode");
            return;
        }

        var boxColliders = root.GetComponentsInChildren<BoxCollider>(true);
        if (boxColliders.Length == 0)
        {
            Debug.LogWarning("没有找到 BoxCollider");
            return;
        }

        StringBuilder sb = new StringBuilder();

        string FormatFloat(float value)
        {
            // 确保输出形如 "11.0f" 或 "-3.25f"
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


        // 导出到项目外层
        string path = Path.Combine(Application.dataPath, "../ExportedCollisionAABB.cpp");
        File.WriteAllText(path, sb.ToString(), Encoding.UTF8);

        Debug.Log($"成功导出 {boxColliders.Length} 个 BoxCollider 到: {path}");
        EditorUtility.RevealInFinder(path);
    }
}
