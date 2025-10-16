#ifndef __LHSMT_TRIANGLE_H__
#define __LHSMT_TRIANGLE_H__

#include "lhsMT_locafx.h"
#include "lhsMT_vec2.h"
#include "lhsMT_vec3.h"
#include "lhsMT_size.h"
#include "lhsMT_rect.h"
#include "lhsMT_ray.h"
#include "lhsMT_matrix4.h"

FW_NS_BEGIN

namespace triangle {

	/* not contains edge */
	bool FW_DLL triangle_2D_TriangleContainsPoint(const Vec2 * vTriangle, const Vec2 & point);

	/* contains edge */
	bool FW_DLL triangle_2D_TriangleContainsPoint2(const Vec2 * vTriangle, const Vec2 & point);
	
	/*
	determine whether a ray intersect with a triangle
	parameters
	ray: Ray
	vTriangle: vertices of triangle ,type array[3]
	t(out): weight of the intersection for the ray
	u(out), v(out): barycentric coordinate of intersection
	*/
	bool FW_DLL triangle_3D_IntersectionInfoOfRayAndTriangle(const Ray & ray, const Vec3 * vTriangle, float * out_t, float * out_u, float * out_v);

}

FW_NS_END


#endif // !__LHSMT_TRIANGLE_H__