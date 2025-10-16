#ifndef __LHSMT_AABB_H_
#define __LHSMT_AABB_H_

#include "lhsMT_locafx.h"
#include "lhsMT_vec3.h"
#include "lhsMT_ray.h"
#include "lhsMT_plane.h"


FW_NS_BEGIN


/*
axis aligned bounding box
   |+y
   |
   |		+x
   +--------
  /
 /+z
    +-------+
   /|      /|
  / |     / |
 +------max |
 | min---|--+
 | /     | /
 |/      |/
 +-------+
*/

class BoundingSphere;
class FW_DLL AABB
{
public:
	AABB();
	AABB(const Vec3 & min, const Vec3 & max);

	void set(const Vec3 & min, const Vec3 & max);

	bool intersects(const AABB & aabb) const;
	
	bool intersects(const Ray & ray) const;

	int intersects(const Plane & plane) const;

	bool containPoint(const Vec3 & point) const;

	/*
           7-------4
          /|      /|
         / |     / |
        0-------3  |
        |  6----|--5
        | /     | /
        |/      |/
        1-------2
	verts[0] : left top front
	verts[1] : left bottom front
	verts[2] : right bottom front
	verts[3] : right top front
	verts[4] : right top back
	verts[5] : right bottom back
	verts[6] : left bottom back
	verts[7] : left top back
	*/
	bool intersectsOrientedBoundingBox(const Vec3 * verts) const;

	/*
           7-------4
          /|      /|
         / |     / |
        0-------3  |
        |  6----|--5
        | /     | /
        |/      |/
        1-------2
	verts[0] : left top front
	verts[1] : left bottom front
	verts[2] : right bottom front
	verts[3] : right top front
	verts[4] : right top back
	verts[5] : right bottom back
	verts[6] : left bottom back
	verts[7] : left top back
	*/
	void getCorners(Vec3 * verts) const;

	Vec3 getCenter() const;

	void merge(const AABB & box);

	void merge(const BoundingSphere & sphere);

	void transform(const Matrix4 & m);

	void output() const;

public:
	Vec3 _getEdgeDirection(const Vec3 * verts, int index) const;
	Vec3 _getFaceDirection(const Vec3 * verts, int index) const;
	
public:
	Vec3 _min;
	Vec3 _max;
};


FW_NS_END

#endif //!__LHSMT_AABB_H_
