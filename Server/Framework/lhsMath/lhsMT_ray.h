#ifndef __LHSMT_RAY_H_
#define __LHSMT_RAY_H_

#include "lhsMT_locafx.h"
#include "lhsMT_vec3.h"
#include "lhsMT_matrix4.h"


FW_NS_BEGIN


class FW_DLL Ray
{
public:
	static const int INTERSECTS_NONE = 0;
	static const int INTERSECTS_ONE = 1;
	static const int INTERSECTS_INFINITY = 2;

	static const int TYPE_ONE_WAY = 0;
	static const int TYPE_TWO_WAY = 1;

public:
	Ray();
	Ray(const Vec3 & origin, const Vec3 & direction);
	Ray(const Vec3 & origin, const Vec3 & direction, int type);

	void set(const Vec3 & origin, const Vec3 & direction);

	const Vec3 & getOrigin() const;

	const Vec3 & getDirection() const;

	void setRayType(int type);

	int getRayType() const;

	/* include cross and different planes */
	bool isVerticalOfTwoLines(const Ray & ray) const;

	/* include overlap , "out_distance" only when return true */
	bool isParallelOfTwoLines(const Ray & ray, float * out_distance = nullptr) const;

	/* "out_distance" only when return true */
	bool isDifferentPlanesOfTwoLines(const Ray & ray, float * out_distance = nullptr) const;

	/* point to ray distance */
	float toPointDistance(const Vec3 & point) const;

	/*
	return value type: INTERSECTS_NONE or INTERSECTS_ONE or INTERSECTS_INFINITY .
	"out" only when return INTERSECTS_ONE .
	*/
	int crossOfTwoLines(const Ray & ray, Vec3 * out_intersection = nullptr) const;

	/* cross of ray and rectangle */
	bool crossOfRayAndRectangle(const Vec3 & rect_lt, const Vec3 & rect_lb, const Vec3 & rect_rb, Vec3 * out_intersection = nullptr) const;

	void transform(const Matrix4 & m);

	void output() const;

private:
	Vec3 _origin;
	Vec3 _direction;	//normalize
	int _type;			//default value TYPE_ONE_WAY
};



FW_NS_END


#endif //!__LHSMT_RAY_H_
