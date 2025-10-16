#pragma once
#include "../LogicalBase/Local.h"
#include "../LogicalBase/TypeDefine.h"
#include "HandlerTypeDef.h"
#include "../LogicalNetwork/Message/Message.h"


class BVHNode
{
public:
    static const int itemMaxCount = 4;
    ~BVHNode() { if (left) delete left; if (right) delete right; }
    Framework::AABB bounds;
    BVHNode* left = nullptr;
    BVHNode* right = nullptr;
    Framework::AABB items[itemMaxCount]; // Ҷ�ӽڵ�洢
    unsigned char itemCount = 0;
    bool isLeaf() const { return itemCount > 0; }
};

class GameMap
{
public:
	GameMap();
	~GameMap();

public:
    // out_contactPoint �ǽ�������ĳһ��
    bool bvhIntersects_Capsule_1(const Framework::Capsule& capsule, float* out_t_on_p0_p1, Framework::Vec3* out_cp_on_p0_p1) const;

private:
	std::vector<Framework::AABB> _gameMapBuildingCollisionAABBs; // ��̬����
    BVHNode* _bvhRoot; // ��̬���������BVH
};
