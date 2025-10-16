#ifndef __LHSMT_FRUSTUM_H__
#define __LHSMT_FRUSTUM_H__

#include "lhsMT_locafx.h"
#include "lhsMT_vec3.h"
#include "lhsMT_plane.h"
#include "lhsMT_aabb.h"
#include "lhsMT_boundingSphere.h"
#include "lhsMT_matrix4.h"


FW_NS_BEGIN

class FW_DLL Frustum
{
public:
	Frustum();

	Frustum(const Matrix4 & matrix4);

	void set(const Matrix4 & matrix4);
	
	/*
	verts[0] : left top near
	verts[1] : left bottom near
	verts[2] : right bottom near
	verts[3] : right top near
	verts[4] : right top far
	verts[5] : right bottom far
	verts[6] : left bottom far
	verts[7] : left top far
	*/
	void getCorners(Vec3 * corners) const;

	bool intersects(const Vec3 & point) const;

	bool intersects(const BoundingSphere & sphere) const;

	bool intersects(const AABB & aabb) const;

	int intersects(const Plane & plane) const;

	bool intersects(const Ray & ray) const;

	void output() const;

public:

	/*
	Updates the planes of the frustum.
	*/
	void updatePlanes();

	/*
	6个平面都朝向视锥体内部
	*/
	Plane _near;
	Plane _far;
	Plane _bottom;
	Plane _top;
	Plane _left;
	Plane _right;
	Matrix4 _matrix4;
};




FW_NS_END


#endif // !__LHSMT_FRUSTUM_H__