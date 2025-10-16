#ifndef __LHSMT_RECT_H__
#define __LHSMT_RECT_H__

#include "lhsMT_locafx.h"
#include "lhsMT_vec2.h"
#include "lhsMT_vec3.h"
#include "lhsMT_size.h"


FW_NS_BEGIN


class FW_DLL Rect
{
public:
	Rect();
	Rect(float x, float y, float w, float h);
	Rect(const Vec2 & pos, const Size & dimension);

	void set(float x, float y, float w, float h);

	void set(const Vec2 & pos, const Size & dimension);

	float getMinX() const;

	float getMidX() const;

	float getMaxX() const;

	float getMinY() const;

	float getMidY() const;

	float getMaxY() const;

	bool equals(const Rect& v) const;

	bool containsPoint(const Vec2& v) const;

	bool intersectsRect(const Rect& v) const;

	bool intersectsCircle(const Vec2& c, float r) const;
	
	void merge(const Rect& v);

	void smallest(const Rect& v);

	void output() const;

public:
	Vec2 origin;
	Size  size;
};


FW_NS_END


#endif // !__LHSMT_RECT_H__