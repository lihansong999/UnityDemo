#ifndef __LHSMT_SIZE_H__
#define __LHSMT_SIZE_H__

#include "lhsMT_locafx.h"


FW_NS_BEGIN

class FW_DLL Size
{
public:
	Size();
	Size(float w, float h);

	Size operator+(const Size& s) const;
	Size operator-(const Size& s) const;
	Size operator*(float v) const;
	Size operator/(float v) const;

	void set(float w, float h);

	bool equals(const Size & v) const;

	void output() const;

public:
	float width;
	float height;
};




FW_NS_END


#endif // !__LHSMT_SIZE_H__