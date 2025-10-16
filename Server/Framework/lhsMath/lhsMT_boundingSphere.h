#ifndef __LHSMT_BOUNDINGSPHERE_H_
#define __LHSMT_BOUNDINGSPHERE_H_

#include "lhsMT_locafx.h"
#include "lhsMT_vec3.h"
#include "lhsMT_aabb.h"
#include "lhsMT_rect.h"
#include "lhsMT_plane.h"

FW_NS_BEGIN

class FW_DLL BoundingSphere
{
public:
	BoundingSphere();

	BoundingSphere(const Vec3 & center, float radius);

	void set(const Vec3 & center, float radius);

	bool intersects(const Vec3 & p) const;

	bool intersects(const BoundingSphere & sphere) const;

	bool intersects(const AABB & aabb) const;

	bool intersects(const Ray & ray) const;

	int intersects(const Plane & plane) const;

	void merge(const BoundingSphere & sphere);

	void merge(const AABB & box);

	void transform(const Matrix4 & m);

	void output() const;

public:
	Vec3 _center;
	float _radius;
};




FW_NS_END


#endif //!__LHSMT_BOUNDINGSPHERE_H_
