#ifndef __LHSMT_CAPSULE_H_
#define __LHSMT_CAPSULE_H_

#include "lhsMT_locafx.h"
#include "lhsMT_vec3.h"
#include "lhsMT_aabb.h"


FW_NS_BEGIN


class FW_DLL Capsule
{
public:
    Capsule();
    Capsule(const Vec3& p0, const Vec3& p1, float radius);
    Capsule(const Vec3& center, float radius, float height);

    void set(const Vec3& p0, const Vec3& p1, float radius);
    void set(const Vec3& center, float radius, float height);

    const Vec3& getP0() const;

    const Vec3& getP1()   const;

    float getRadius() const;

    bool intersects(const AABB& box, float* out_t_on_p0_p1, Vec3* out_cp_on_p0_p1) const;
    bool intersects(const Capsule& other, float* out_t_on_this, float* out_t_on_other, Vec3* out_cp_this, Vec3* out_cp_other) const;
    bool intersectsOrBehindPlane(const Plane& plane, float* out_t_on_p0_p1, Vec3* out_cp_on_p0_p1) const;

    void output() const;

private:
    Vec3 _p0;
    Vec3 _p1;
    float _radius;
};


FW_NS_END

#endif // __LHSMT_CAPSULE_H_
