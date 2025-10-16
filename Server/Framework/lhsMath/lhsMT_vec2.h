#ifndef __LHSMT_VEC2_H__
#define __LHSMT_VEC2_H__

#include "lhsMT_locafx.h"

FW_NS_BEGIN

class FW_DLL Vec2
{
public:
	Vec2();
	Vec2(float xx, float yy);

	Vec2 operator+(const Vec2 & v) const;
	Vec2 operator-(const Vec2 & v) const;
	Vec2 operator+(float v) const;
	Vec2 operator-(float v) const;
	Vec2 operator*(float v) const;
	Vec2 operator/(float v) const;
	float operator[](int i) const;

	void set(float xx, float yy);

	bool isZero() const;

	Vec2 normalize() const;

	float length() const;

	float lengthSquared() const;

	Vec2 negate() const;

	float dot(const Vec2 & v) const;

	float cross(const Vec2 & v) const;

	float radian(const Vec2 & v) const;	/* call function cross() get signed */

	float angle(const Vec2 & v) const;	/* call function cross() get signed */

	bool equals(const Vec2 & v) const;

	void setlength(float l);

	void output() const;

public:
	float x;
	float y;
};


FW_NS_END


#endif // !__LHSMT_VEC2_H__