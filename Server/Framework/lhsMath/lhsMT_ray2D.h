#ifndef __LHSMT_RAY2D_H_
#define __LHSMT_RAY2D_H_

#include "lhsMT_locafx.h"
#include "lhsMT_vec2.h"

FW_NS_BEGIN

class FW_DLL Ray2D
{
public:
	static const int INTERSECTS_NONE = 0;
	static const int INTERSECTS_ONE = 1;
	static const int INTERSECTS_INFINITY = 2;

	static const int TYPE_ONE_WAY = 0;
	static const int TYPE_TWO_WAY = 1;

public:
	Ray2D();
	Ray2D(const Vec2 & origin, const Vec2 & direction);
	Ray2D(const Vec2 & origin, const Vec2 & direction, int type);

	void set(const Vec2 & origin, const Vec2 & direction);

	const Vec2 & getOrigin() const;

	const Vec2 & getDirection() const;

	void setRayType(int type);

	int getRayType() const;

	/* include cross */
	bool isVerticalOfTwoLines(const Ray2D & ray) const;

	/* include overlap , "out_distance" only when return true */
	bool isParallelOfTwoLines(const Ray2D & ray, float * out_distance = nullptr) const;

	/* point to ray distance */
	float toPointDistance(const Vec2 & point) const;

	/*
	return value type: INTERSECTS_NONE or INTERSECTS_ONE or INTERSECTS_INFINITY .
	"out" only when return INTERSECTS_ONE .
	*/
	int crossOfTwoLines(const Ray2D & ray, Vec2 * out_intersection = nullptr) const;
	
	void output() const;

private:
	Vec2 _origin;
	Vec2 _direction;	//normalize
	int _type;			//default value TYPE_ONE_WAY
};


FW_NS_END


#endif // !__LHSMT_RAY2D_H_