#ifndef __LHSMT_PLANE_H__
#define __LHSMT_PLANE_H__

#include "lhsMT_locafx.h"
#include "lhsMT_vec3.h"
#include "lhsMT_matrix4.h"
#include "lhsMT_ray.h"

FW_NS_BEGIN

/** 
Defines a plane in 3D space.
@remarks
A plane is defined in 3D space by the equation
Ax + By + Cz + D = 0
Ax + By + Cz + -(Ax0 + By0 + Cz0) = 0
@par
This equates to a vector (the normal of the plane, whose x, y
and z components equate to the coefficients A, B and C
respectively), and a constant (D) which is the distance along
the normal you have to go to move the plane back to the origin.
*/
class FW_DLL Plane
{
public:
	static const int INTERSECTS_INTERSECTING = 0;
	static const int INTERSECTS_FRONT = 1;
	static const int INTERSECTS_BACK = -1;
public:
	Plane();
	Plane(const Vec3 & normal, const Vec3 & point); /* 用这个更容易理解 */
	Plane(const Vec3 & point0, const Vec3 & point1, const Vec3 & point2);
	Plane(const Vec3 & normal, float distance);

	void set(const Vec3 & normal, const Vec3 & point);
	void set(const Vec3 & point0, const Vec3 & point1, const Vec3 & point2);
	void set(const Vec3 & normal, float distance);
	void setWithNotNormalize(const Vec3 & normal, float distance);

	float getDistance() const;
	const Vec3 & getNormal() const;
	
	Vec3 getIntersectionOfOriginPointToPlane() const;

	float getPointToPlaneDistance(const Vec3 & point) const;

	bool isParallel(const Plane & plane) const;

	int intersects(const Ray & ray) const;

	int intersects(const Plane & plane) const;

	void transform(const Matrix4 & m);

	void output() const;

public:
	/**
	* Calculates the point of intersection of the given three planes and stores it in the given point.
	*
	* @param p1 The first plane.
	* @param p2 The second plane.
	* @param p3 The third plane.
	* @param point The point to store the point of intersection in
	*  (this is left unmodified if the planes do not all intersect or if
	*  they are all parallel along one vector and intersect along a line).
	* succeed return true
	*/
	static bool intersection(const Plane & p1, const Plane & p2, const Plane & p3, Vec3 * point);

private:

	void normalize();

	Vec3 _normal;		// the normal vector of the Plane.
	float _distance;    // the distance of the Plane along its normal from the origin.(stored as a negative value -(Ax0 + By0 + Cz0) ). 为原点到平面的垂直距离(与法线共线).
};



FW_NS_END


#endif // !__LHSMT_PLANE_H__