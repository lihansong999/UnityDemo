#ifndef __LHSMT_QUATERNION_H_
#define __LHSMT_QUATERNION_H_

#include "lhsMT_locafx.h"
#include "lhsMT_matrix4.h"

FW_NS_BEGIN

/**
* Defines a 4-element quaternion that represents the orientation of an object in space.
*
* Quaternions are typically used as a replacement for euler angles and rotation matrices as a way to achieve smooth interpolation and avoid gimbal lock.
*
* Note that this quaternion class does not automatically keep the quaternion normalized. Therefore, care must be taken to normalize the quaternion when necessary, by calling the normalize method.
* This class provides three methods for doing quaternion interpolation: lerp, slerp, and squad.
*
* lerp (linear interpolation): the interpolation curve gives a straight line in quaternion space. It is simple and fast to compute. The only problem is that it does not provide constant angular velocity. Note that a constant velocity is not necessarily a requirement for a curve;
* slerp (spherical linear interpolation): the interpolation curve forms a great arc on the quaternion unit sphere. Slerp provides constant angular velocity;
* squad (spherical spline interpolation): interpolating between a series of rotations using slerp leads to the following problems:
* - the curve is not smooth at the control points;
* - the angular velocity is not constant;
* - the angular velocity is not continuous at the control points.
*
* Since squad is continuously differentiable, it remedies the first and third problems mentioned above.
* The slerp method provided here is intended for interpolation of principal rotations. It treats +q and -q as the same principal rotation and is at liberty to use the negative of either input. The resulting path is always the shorter arc.
*
* The lerp method provided here interpolates strictly in quaternion space. Note that the resulting path may pass through the origin if interpolating between a quaternion and its exact negative.
*
* As an example, consider the following quaternions:
*
* q1 = (0.6, 0.8, 0.0, 0.0),
* q2 = (0.0, 0.6, 0.8, 0.0),
* q3 = (0.6, 0.0, 0.8, 0.0), and
* q4 = (-0.8, 0.0, -0.6, 0.0).
* For the point p = (1.0, 1.0, 1.0), the following figures show the trajectories of p using lerp, slerp, and squad.
*/

/*
ijk分别为三个虚数单位
q = w + xi + yj + zk
*/

class FW_DLL Quaternion
{
public:
	Quaternion();
	Quaternion(float xx, float yy, float zz, float ww);
	Quaternion(const Matrix4 & matrix4);
	Quaternion(const Vec3 & axis, float angle);
	Quaternion(float yawAngle, float pitchAngle, float rollAngle);

	void set(float xx, float yy, float zz, float ww);
	void set(const Matrix4 & matrix4);
	void set(const Vec3 & axis, float angle);
	/*
	yawAngle axis Vec3(0.0f, 1.0f, 0.0f)
	pitchAngle axis Vec3(1.0f, 0.0f, 0.0f)
	rollAngle axis Vec3(0.0f, 0.0f, 1.0f)
	euler z->x->y
	*/
	void set(float yawAngle, float pitchAngle, float rollAngle);

	Quaternion operator*(const Quaternion & q) const;
	Quaternion operator*(float v) const;
	Quaternion & operator*=(const Quaternion & q);
	Quaternion & operator*=(float v);
	Quaternion operator+ (const Quaternion & q) const;
	Quaternion operator- (const Quaternion & q) const;
	Quaternion operator- () const;

	float length() const;

	float lengthSquared() const;

	float dot(const Quaternion & q) const;

	Quaternion exp() const;

	Quaternion log() const;

	Quaternion conjugate() const;

	Quaternion invert() const;
	
	Quaternion multiply(const Quaternion & q) const;

	Quaternion normalize() const;

	float toAxisAngle(Vec3 * axis) const;

	void computeEuler(float* yawAngle, float* pitchAngle, float* rollAngle) const;
	
	Vec3 rotatePoint(const Vec3 & point) const;

	void output() const;
	
	/*
	copy from ogre
	Performs Normalised linear interpolation between two quaternions, and returns the result.
	nlerp ( 0.0f, A, B ) = A
	nlerp ( 1.0f, A, B ) = B
	@remarks
	Nlerp is faster than Slerp.
	Nlerp has the proprieties of being commutative (@see Slerp;
	commutativity is desired in certain places, like IK animation), and
	being torque-minimal (unless shortestPath=false). However, it's performing
	the interpolation at non-constant velocity; sometimes this is desired,
	sometimes it is not. Having a non-constant velocity can produce a more
	natural rotation feeling without the need of tweaking the weights; however
	if your scene relies on the timing of the rotation or assumes it will point
	at a specific angle at a specific weight value, Slerp is a better choice.
	*/
	static Quaternion nlerp_ogre(const Quaternion & q1, const Quaternion & q2, float t, bool shortestPath = false);
	
	/*
	copy from ogre
	Performs Spherical linear interpolation between two quaternions, and returns the result.
	Slerp ( 0.0f, A, B ) = A
	Slerp ( 1.0f, A, B ) = B
	@return Interpolated quaternion
	@remarks
	Slerp has the proprieties of performing the interpolation at constant
	velocity, and being torque-minimal (unless shortestPath=false).
	However, it's NOT commutative, which means
	Slerp ( 0.75f, A, B ) != Slerp ( 0.25f, B, A );
	therefore be careful if your code relies in the order of the operands.
	This is specially important in IK animation.
	*/
	static Quaternion slerp_ogre(const Quaternion& q1, const Quaternion& q2, float t, bool shortestPath = false);

	/*
	copy from ogre
	Spherical quadratic interpolation
	*/
	static Quaternion squad_ogre(const Quaternion & q1, const Quaternion & q2, const Quaternion & s1, const Quaternion & s2, float t, bool shortestPath = false);

	/**
	* Interpolates between two quaternions using linear interpolation.
	*
	* The interpolation curve for linear interpolation between
	* quaternions gives a straight line in quaternion space.
	*
	* The lerp method provided here interpolates strictly in quaternion space. 
	* Note that the resulting path may pass through the origin if interpolating between a quaternion and its exact negative.
	*
	* @param q1 The first quaternion.
	* @param q2 The second quaternion.
	* @param t The interpolation coefficient.
	*/
	static Quaternion lerp(const Quaternion & q1, const Quaternion & q2, float t);

	/**
	* Interpolates between two quaternions using spherical linear interpolation.
	*
	* Spherical linear interpolation provides smooth transitions between different
	* orientations and is often useful for animating models or cameras in 3D.
	*
	* Note: For accurate interpolation, the input quaternions must be at (or close to) unit length.
	*
	* The slerp method provided here is intended for interpolation of principal rotations. 
	* It treats +q and -q as the same principal rotation and is at liberty to use the negative of either input. 
	* The resulting path is always the shorter arc.
	*
	* @param q1 The first quaternion.
	* @param q2 The second quaternion.
	* @param t The interpolation coefficient.
	*/
	static Quaternion slerp(const Quaternion & q1, const Quaternion & q2, float t);

	/**
	* Interpolates over a series of quaternions using spherical spline interpolation.
	*
	* Spherical spline interpolation provides smooth transitions between different
	* orientations and is often useful for animating models or cameras in 3D.
	*
	* Note: For accurate interpolation, the input quaternions must be unit.
	*
	* @param q1 The first quaternion.
	* @param q2 The second quaternion.
	* @param s1 The first control point.
	* @param s2 The second control point.
	* @param t The interpolation coefficient.
	*/
	static Quaternion squad(const Quaternion & q1, const Quaternion & q2, const Quaternion & s1, const Quaternion & s2, float t);

	/*
	Setup for spherical quadratic interpolation
	return si
	*/
	static Quaternion intermediate(const Quaternion & qiSub1, const Quaternion & qi, const Quaternion & qiAdd1);

private:
	static Quaternion slerpForSquad(const Quaternion & q1, const Quaternion & q2, float t);

public:
	float x;
	float y;
	float z;
	float w;
};




FW_NS_END


#endif //!__LHSMT_QUATERNION_H_
