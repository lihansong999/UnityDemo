#ifndef __LHSMT_AFFINE_TRANSFORM_H__
#define __LHSMT_AFFINE_TRANSFORM_H__

#include "lhsMT_locafx.h"
#include "lhsMT_vec2.h"
#include "lhsMT_size.h"
#include "lhsMT_rect.h"

/*
2D 
3*3 matrix
affine transform
*/

FW_NS_BEGIN

namespace affine {

	/*
	| a   c   tx|
	| b   d   ty|
	| 0   0   1 |
	*/
	struct AffineTransform {
		float a, b, c, d;
		float tx, ty;
	};

	AffineTransform FW_DLL affine_TransformMake(float a, float b, float c, float d, float tx, float ty);

	AffineTransform FW_DLL affine_TransformMakeIdentity();

	Vec2 FW_DLL affine_vec2ApplyAffineTransform(const Vec2 & point, const AffineTransform & t);

	Size FW_DLL affine_sizeApplyAffineTransform(const Size& size, const AffineTransform& t);

	Rect FW_DLL affine_rectApplyAffineTransform(const Rect & rect, const AffineTransform& anAffineTransform);

	AffineTransform FW_DLL affine_TransformTranslate(const AffineTransform & t, float tx, float ty);

	AffineTransform FW_DLL affine_TransformRotate(const AffineTransform & t, float radian);

	AffineTransform FW_DLL affine_TransformScale(const AffineTransform & t, float sx, float sy);

	/* (t2) left mul (t1) */
	AffineTransform FW_DLL affine_TransformConcat(const AffineTransform & t1, const AffineTransform & t2);

	AffineTransform FW_DLL affine_TransformInvert(const AffineTransform & t);

}

FW_NS_END


#endif // !__LHSMT_AFFINE_TRANSFORM_H__