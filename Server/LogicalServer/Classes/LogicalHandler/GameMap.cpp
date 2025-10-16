#include "stdafx.h"
#include "GameMap.h"

FW_NS_USEING;


inline BVHNode* BuildBVH(const std::vector<Framework::AABB>& aabbs, int start, int end)
{
	BVHNode* node = new BVHNode();

	// 计算包围盒
	node->bounds = aabbs[start];
	for (int i = start + 1; i < end; ++i)
		node->bounds.merge(aabbs[i]);

	int count = end - start;
	if (count <= BVHNode::itemMaxCount) { // 小于等于4个 AABB 作为叶子
		for (int i = start; i < end; ++i)
			node->items[node->itemCount++] = aabbs[i];
		return node;
	}

	// 按最长轴分割
	Framework::Vec3 size = node->bounds._max - node->bounds._min;
	int axis = 0;
	if (size.y > size.x && size.y > size.z) axis = 1;
	else if (size.z > size.x) axis = 2;

	// 排序
	std::vector<const Framework::AABB*> sortedAABBs;
	for (int i = start; i < end; ++i)
		sortedAABBs.push_back(&aabbs[i]);
	std::sort(sortedAABBs.begin(), sortedAABBs.end(), [axis](const Framework::AABB* a, const Framework::AABB* b) {
		return (a->_min[axis] + a->_max[axis]) * 0.5f < (b->_min[axis] + b->_max[axis]) * 0.5f;
		});

	int mid = count / 2;
	std::vector<Framework::AABB> leftAABBs, rightAABBs;
	for (int i = 0; i < mid; ++i) leftAABBs.push_back(*sortedAABBs[i]);
	for (int i = mid; i < count; ++i) rightAABBs.push_back(*sortedAABBs[i]);

	node->left = BuildBVH(leftAABBs, 0, (int)leftAABBs.size());
	node->right = BuildBVH(rightAABBs, 0, (int)rightAABBs.size());

	return node;
}

GameMap::GameMap()
{
	_gameMapBuildingCollisionAABBs.push_back(AABB(Vec3(-62.81f, 0.139999f, -98.39f), Vec3(-29.21f, 16.34f, -81.09f)));
	_gameMapBuildingCollisionAABBs.push_back(AABB(Vec3(43.99f, 0.1f, -116.96f), Vec3(61.79f, 15.1f, -98.76f)));
	_gameMapBuildingCollisionAABBs.push_back(AABB(Vec3(80.02f, 0.1f, -134.04f), Vec3(98.02f, 15.1f, -116.34f)));
	_gameMapBuildingCollisionAABBs.push_back(AABB(Vec3(62.29f, 0.1f, -170.24f), Vec3(79.79f, 15.1f, -154.64f)));
	_gameMapBuildingCollisionAABBs.push_back(AABB(Vec3(79.31f, 0.1f, -170.8f), Vec3(98.31f, 15.1f, -152.3f)));
	_gameMapBuildingCollisionAABBs.push_back(AABB(Vec3(61.64f, 0.1f, -206.66f), Vec3(97.84f, 15.1f, -188.16f)));
	_gameMapBuildingCollisionAABBs.push_back(AABB(Vec3(117.38f, -0.16f, -205.79f), Vec3(132.68f, 29.84f, -188.29f)));
	_gameMapBuildingCollisionAABBs.push_back(AABB(Vec3(117.38f, -0.16f, -170.75f), Vec3(132.68f, 29.84f, -153.25f)));
	_gameMapBuildingCollisionAABBs.push_back(AABB(Vec3(119.27f, -0.299999f, -189.0f), Vec3(130.67f, 27.7f, -170.0f)));
	_gameMapBuildingCollisionAABBs.push_back(AABB(Vec3(118.37f, -0.299999f, -182.1f), Vec3(131.57f, 27.7f, -176.9f)));
	_gameMapBuildingCollisionAABBs.push_back(AABB(Vec3(118.24f, 0.1f, -134.3f), Vec3(133.84f, 15.1f, -116.3f)));
	_gameMapBuildingCollisionAABBs.push_back(AABB(Vec3(115.85f, 0.1f, -116.92f), Vec3(134.25f, 15.1f, -98.12f)));
	_gameMapBuildingCollisionAABBs.push_back(AABB(Vec3(121.35f, 0.12f, -98.85f), Vec3(133.65f, 6.12f, -87.85f)));
	_gameMapBuildingCollisionAABBs.push_back(AABB(Vec3(187.75f, -0.16f, -133.2f), Vec3(205.25f, 29.84f, -117.9f)));
	_gameMapBuildingCollisionAABBs.push_back(AABB(Vec3(152.79f, -0.16f, -133.1f), Vec3(170.29f, 29.84f, -117.8f)));
	_gameMapBuildingCollisionAABBs.push_back(AABB(Vec3(169.48f, -0.299999f, -131.2f), Vec3(188.48f, 27.7f, -119.8f)));
	_gameMapBuildingCollisionAABBs.push_back(AABB(Vec3(176.41f, -0.299999f, -132.08f), Vec3(181.61f, 27.7f, -118.88f)));
	_gameMapBuildingCollisionAABBs.push_back(AABB(Vec3(151.59f, 0.1f, -98.55f), Vec3(187.79f, 15.1f, -80.05f)));
	_gameMapBuildingCollisionAABBs.push_back(AABB(Vec3(188.14f, 0.1f, -116.96f), Vec3(205.94f, 15.1f, -98.76f)));
	_gameMapBuildingCollisionAABBs.push_back(AABB(Vec3(188.14f, 0.1f, -98.51f), Vec3(205.94f, 15.1f, -80.31001f)));
	_gameMapBuildingCollisionAABBs.push_back(AABB(Vec3(205.44f, 0.1f, -98.59f), Vec3(224.24f, 15.1f, -80.19f)));
	_gameMapBuildingCollisionAABBs.push_back(AABB(Vec3(187.84f, 0.1f, -170.73f), Vec3(206.34f, 15.1f, -151.73f)));
	_gameMapBuildingCollisionAABBs.push_back(AABB(Vec3(242.12f, 0.1f, -98.37f), Vec3(260.12f, 15.1f, -82.77f)));
	_gameMapBuildingCollisionAABBs.push_back(AABB(Vec3(259.87f, 0.1f, -99.2f), Vec3(278.37f, 15.1f, -80.2f)));
	_gameMapBuildingCollisionAABBs.push_back(AABB(Vec3(331.79f, -0.16f, -97.19f), Vec3(349.29f, 29.84f, -81.89f)));
	_gameMapBuildingCollisionAABBs.push_back(AABB(Vec3(296.83f, -0.16f, -97.09f), Vec3(314.33f, 29.84f, -81.78999f)));
	_gameMapBuildingCollisionAABBs.push_back(AABB(Vec3(313.52f, -0.299999f, -95.18999f), Vec3(332.52f, 27.7f, -83.79f)));
	_gameMapBuildingCollisionAABBs.push_back(AABB(Vec3(320.45f, -0.299999f, -96.07f), Vec3(325.65f, 27.7f, -82.87f)));
	_gameMapBuildingCollisionAABBs.push_back(AABB(Vec3(358.288f, 0.032f, -115.529f), Vec3(358.638f, 4.032f, -115.179f)));
	_gameMapBuildingCollisionAABBs.push_back(AABB(Vec3(358.351f, 0.032f, -81.83501f), Vec3(358.701f, 4.032f, -81.485f)));
	_gameMapBuildingCollisionAABBs.push_back(AABB(Vec3(88.305f, 0.032f, -115.529f), Vec3(88.65501f, 4.032f, -115.179f)));
	_gameMapBuildingCollisionAABBs.push_back(AABB(Vec3(88.36799f, 0.032f, -81.83501f), Vec3(88.71799f, 4.032f, -81.485f)));
	_gameMapBuildingCollisionAABBs.push_back(AABB(Vec3(152.97f, 0.032f, -108.144f), Vec3(153.32f, 4.032f, -107.794f)));
	_gameMapBuildingCollisionAABBs.push_back(AABB(Vec3(186.679f, 0.032f, -108.209f), Vec3(187.029f, 4.032f, -107.859f)));
	_gameMapBuildingCollisionAABBs.push_back(AABB(Vec3(-8.672f, -0.12f, -79.195f), Vec3(-8.322f, 9.88f, -78.84499f)));
	_gameMapBuildingCollisionAABBs.push_back(AABB(Vec3(-8.674f, -0.12f, -64.195f), Vec3(-8.323999f, 9.88f, -63.84499f)));
	_gameMapBuildingCollisionAABBs.push_back(AABB(Vec3(6.335f, -0.12f, -79.165f), Vec3(6.685f, 9.88f, -78.81499f)));
	_gameMapBuildingCollisionAABBs.push_back(AABB(Vec3(6.333f, -0.12f, -64.165f), Vec3(6.683f, 9.88f, -63.81499f)));
	_gameMapBuildingCollisionAABBs.push_back(AABB(Vec3(47.778f, -0.12f, -64.174f), Vec3(48.128f, 9.88f, -63.824f)));
	_gameMapBuildingCollisionAABBs.push_back(AABB(Vec3(63.317f, -0.12f, -79.16501f), Vec3(63.667f, 9.88f, -78.815f)));
	_gameMapBuildingCollisionAABBs.push_back(AABB(Vec3(63.315f, -0.12f, -64.165f), Vec3(63.665f, 9.88f, -63.815f)));
	_gameMapBuildingCollisionAABBs.push_back(AABB(Vec3(78.324f, -0.12f, -79.13501f), Vec3(78.674f, 9.88f, -78.785f)));
	_gameMapBuildingCollisionAABBs.push_back(AABB(Vec3(78.322f, -0.12f, -64.135f), Vec3(78.672f, 9.88f, -63.785f)));
	_gameMapBuildingCollisionAABBs.push_back(AABB(Vec3(135.325f, -0.12f, -79.195f), Vec3(135.675f, 9.88f, -78.84499f)));
	_gameMapBuildingCollisionAABBs.push_back(AABB(Vec3(135.323f, -0.12f, -64.19499f), Vec3(135.673f, 9.88f, -63.84499f)));
	_gameMapBuildingCollisionAABBs.push_back(AABB(Vec3(150.332f, -0.12f, -79.165f), Vec3(150.682f, 9.88f, -78.81499f)));
	_gameMapBuildingCollisionAABBs.push_back(AABB(Vec3(150.33f, -0.12f, -64.16499f), Vec3(150.68f, 9.88f, -63.81499f)));
	_gameMapBuildingCollisionAABBs.push_back(AABB(Vec3(178.195f, -0.12f, -64.18501f), Vec3(178.545f, 9.88f, -63.835f)));
	_gameMapBuildingCollisionAABBs.push_back(AABB(Vec3(187.825f, -0.12f, -79.175f), Vec3(188.175f, 9.88f, -78.825f)));
	_gameMapBuildingCollisionAABBs.push_back(AABB(Vec3(208.835f, -0.12f, -79.195f), Vec3(209.185f, 9.88f, -78.84499f)));
	_gameMapBuildingCollisionAABBs.push_back(AABB(Vec3(208.833f, -0.12f, -64.19499f), Vec3(209.183f, 9.88f, -63.84499f)));
	_gameMapBuildingCollisionAABBs.push_back(AABB(Vec3(223.842f, -0.12f, -79.165f), Vec3(224.192f, 9.88f, -78.81499f)));
	_gameMapBuildingCollisionAABBs.push_back(AABB(Vec3(225.325f, -0.12f, -64.16499f), Vec3(225.675f, 9.88f, -63.81499f)));
	_gameMapBuildingCollisionAABBs.push_back(AABB(Vec3(240.325f, -0.12f, -79.165f), Vec3(240.675f, 9.88f, -78.81499f)));
	_gameMapBuildingCollisionAABBs.push_back(AABB(Vec3(240.323f, -0.12f, -64.16499f), Vec3(240.673f, 9.88f, -63.81499f)));
	_gameMapBuildingCollisionAABBs.push_back(AABB(Vec3(259.665f, -0.12f, -64.165f), Vec3(260.015f, 9.88f, -63.815f)));
	_gameMapBuildingCollisionAABBs.push_back(AABB(Vec3(279.325f, -0.12f, -79.18501f), Vec3(279.675f, 9.88f, -78.835f)));
	_gameMapBuildingCollisionAABBs.push_back(AABB(Vec3(279.328f, -0.12f, -64.16499f), Vec3(279.678f, 9.88f, -63.81499f)));
	_gameMapBuildingCollisionAABBs.push_back(AABB(Vec3(294.328f, -0.12f, -79.165f), Vec3(294.678f, 9.88f, -78.81499f)));
	_gameMapBuildingCollisionAABBs.push_back(AABB(Vec3(294.326f, -0.12f, -64.16499f), Vec3(294.676f, 9.88f, -63.81499f)));
	_gameMapBuildingCollisionAABBs.push_back(AABB(Vec3(317.905f, -0.12f, -64.18501f), Vec3(318.255f, 9.88f, -63.835f)));
	_gameMapBuildingCollisionAABBs.push_back(AABB(Vec3(349.335f, -0.12f, -79.18501f), Vec3(349.685f, 9.88f, -78.835f)));
	_gameMapBuildingCollisionAABBs.push_back(AABB(Vec3(-29.295f, -0.12f, -79.18501f), Vec3(-28.945f, 9.88f, -78.835f)));
	_gameMapBuildingCollisionAABBs.push_back(AABB(Vec3(-58.815f, -0.12f, -64.195f), Vec3(-58.465f, 9.88f, -63.84499f)));
	_gameMapBuildingCollisionAABBs.push_back(AABB(Vec3(369.315f, -0.12f, -79.18501f), Vec3(369.665f, 9.88f, -78.835f)));
	_gameMapBuildingCollisionAABBs.push_back(AABB(Vec3(384.318f, -0.12f, -79.165f), Vec3(384.668f, 9.88f, -78.81499f)));
	_gameMapBuildingCollisionAABBs.push_back(AABB(Vec3(384.316f, -0.12f, -64.16499f), Vec3(384.666f, 9.88f, -63.81499f)));

	_bvhRoot = ::BuildBVH(_gameMapBuildingCollisionAABBs, 0, (int)_gameMapBuildingCollisionAABBs.size());
}

GameMap::~GameMap()
{
	if (_bvhRoot)
		delete _bvhRoot;
}

struct QueryCapsule {
	const AABB* _aabb;
	float t;
	Vec3 _cp;
};

inline void QueryBVH_Capsule(const BVHNode* node, const Framework::Capsule& capsule, std::vector<QueryCapsule>& results, bool one)
{
	if (one && results.size() > 0) {
		return;
	}
	if (!capsule.intersects(node->bounds, nullptr, nullptr))
		return;
	if (node->isLeaf()) {
		for (int i = 0; i < node->itemCount; i++) {
			float t;
			Vec3 cp;
			if (capsule.intersects(node->items[i], &t, &cp)) {
				results.push_back({ &(node->items[i]), t, cp });
				if (one)
					return;
			}
		}
	}
	else {
		if (node->left) QueryBVH_Capsule(node->left, capsule, results, one);
		if (node->right) QueryBVH_Capsule(node->right, capsule, results, one);
	}
}

bool GameMap::bvhIntersects_Capsule_1(const Framework::Capsule& capsule, float* out_t_on_p0_p1, Vec3* out_cp_on_p0_p1) const
{
	if (_bvhRoot) {
		std::vector<QueryCapsule> results;
		QueryBVH_Capsule(_bvhRoot, capsule, results, true);
		if (results.size() > 0) {
			*out_t_on_p0_p1 = results[0].t;
			*out_cp_on_p0_p1 = results[0]._cp;
			return true;
		}
	}
	return false;
}