#ifndef __LHSMT_TOOLS_H__
#define __LHSMT_TOOLS_H__

#include "lhsMT_locafx.h"
#include "lhsMT_vec2.h"
#include "lhsMT_vec3.h"
#include "lhsMT_size.h"
#include "lhsMT_rect.h"
#include "lhsMT_matrix4.h"
#include "lhsMT_triangle.h"

FW_NS_BEGIN


namespace tools {

	/*
	|                                   (1,1)|
	|                                        |
	|                                        |
	|                  NDC                   |
	|                                        |
	|                                        |
	|(-1,-1)                                 |
	match
	|        (viewPort.width,viewPort.height)|
	|                                        |
	|                                        |
	|                winPoint                |
	|                                        |
	|                                        |
	|(viewPort.x,viewPort.y)                 |
	*/

	/*
	NDC坐标：x坐标从[l,r]映射到[-1,1]，y坐标从[b,t]映射到[-1,1]，z坐标从[n,f]映射到[-1,1]。
	*/

	/* out.z in the range from 0 to 1 (near to far), unless it is out of NDC */
	bool FW_DLL tools_Project(const Vec3 & objPoint, const Rect & viewPort, const Matrix4 & projMatrix, const Matrix4 & modelMatrix, Vec3 * out);

	/* you can match winPoint.z with function "glDepthRange" ,but must range from 0 to 1 (near to far). */
	bool FW_DLL tools_UnProject(const Vec3 & winPoint, const Rect & viewPort, const Matrix4 & projMatrix, const Matrix4 & modelMatrix, Vec3 * out);

	/* this function uses algorithm that is intersection of ray and plane */
	bool FW_DLL tools_WinPointInModelRect(const Vec2 & winPoint, const Rect & viewPort, const Matrix4 & projMatrix, const Matrix4 & modelMatrix, const Rect & modelRect, Vec2 * out);

	/* this function uses triangle_3D_IntersectionInfoOfBRayAndTriangle */
	bool FW_DLL tools_IntersectionOfWinPointAndTriangle(const Vec2 & winPoint, const Rect & viewPort, const Matrix4 & projMatrix, const Matrix4 & modelMatrix, const Vec3 * vTriangle, Vec3 * out);

	inline int tools_nextPOT(int x)
	{
		x = x - 1;
		x = x | (x >> 1);
		x = x | (x >> 2);
		x = x | (x >> 4);
		x = x | (x >> 8);
		x = x | (x >> 16);
		return x + 1;
	}
	
	inline bool tools_isPOT(int n)
	{
		return ((n > 0) && (n & (n - 1)) == 0);
	}

}

FW_NS_END


#endif // !__LHSMT_TOOLS_H__