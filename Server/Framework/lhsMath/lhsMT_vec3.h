#ifndef __LHSMT_VEC3_H__
#define __LHSMT_VEC3_H__

#include "lhsMT_locafx.h"

FW_NS_BEGIN


class FW_DLL Vec3
{
public:
	Vec3();
	Vec3(float xx, float yy, float zz);

	Vec3 operator+(const Vec3 & v) const;
	Vec3 operator-(const Vec3 & v) const;
	Vec3 operator+(float v) const;
	Vec3 operator-(float v) const;
	Vec3 operator*(float v) const;
	Vec3 operator/(float v) const;
	float operator[](int i) const;

	void set(float xx, float yy, float zz);

	bool isZero() const;

	Vec3 normalize() const;

	float length() const;

	float lengthSquared() const;

	Vec3 negate() const;

	float dot(const Vec3 & v) const;

	Vec3 cross(const Vec3 & v) const;

	float radian(const Vec3 & v) const;

	float angle(const Vec3 & v) const;

	bool equals(const Vec3 & v) const;

	void setlength(float l);

	void output() const;

public:
	float x;
	float y;
	float z;

};



FW_NS_END


#endif // !__LHSMT_VEC3_H__