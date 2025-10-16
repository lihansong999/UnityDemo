#ifndef __LHSMT_MATRIX4_H__
#define __LHSMT_MATRIX4_H__

#include "lhsMT_locafx.h"
#include "lhsMT_vec3.h"

/*
3D 
4*4 matrix
*/

FW_NS_BEGIN

#define MATRIX4_SIZE (sizeof(float)*16)

#define COPY_MATRIX_ASSIMP_2_LOCAL(localMat, assimpMat) \
	do { \
		localMat[0] = assimpMat.a1; localMat[4] = assimpMat.a2; localMat[8] = assimpMat.a3; localMat[12] = assimpMat.a4; \
		localMat[1] = assimpMat.b1; localMat[5] = assimpMat.b2; localMat[9] = assimpMat.b3; localMat[13] = assimpMat.b4; \
		localMat[2] = assimpMat.c1; localMat[6] = assimpMat.c2; localMat[10] = assimpMat.c3; localMat[14] = assimpMat.c4; \
		localMat[3] = assimpMat.d1; localMat[7] = assimpMat.d2; localMat[11] = assimpMat.d3; localMat[15] = assimpMat.d4; \
	} while (false)

class Quaternion;
class Plane;
class FW_DLL Matrix4
{
public:
	Matrix4();

	Matrix4(float m11, float m12, float m13, float m14, float m21, float m22, float m23, float m24,
		float m31, float m32, float m33, float m34, float m41, float m42, float m43, float m44);

	Matrix4(float * m);

	void identity();

	void set(float m11, float m12, float m13, float m14, float m21, float m22, float m23, float m24,
		float m31, float m32, float m33, float m34, float m41, float m42, float m43, float m44);

	void set(float * m);

	void set(int index, float v);

	/* (m) left mul (this) */
	void multiply(const Matrix4 & m);
	
	void multiply(float s);

	void add(const Matrix4 & m);

	void add(float a);

	void sub(const Matrix4 & m);

	void sub(float s);

	bool invert();

	void transpose();

public:
	const Matrix4 operator+(const Matrix4 & m) const;

	Matrix4 & operator+=(const Matrix4 & m);

	const Matrix4 operator-(const Matrix4 & m) const;

	Matrix4 & operator-=(const Matrix4 & m);

	/* (m) left mul (this) */
	const Matrix4 operator*(const Matrix4 & m) const;

	/* (m) left mul (this) */
	Matrix4 & operator*=(const Matrix4 & m);

	const Matrix4 operator*(float s) const;
	
	Matrix4 & operator*=(float s);

public:
	const float * getArray() const;

	bool equal(const Matrix4 & m, float epsilon = LM_EPSILON) const;

	/* (v) left mul (this) */
	Vec3 transformVec3(const Vec3 & v, float w, float * out_w = nullptr) const;

	/* (v) left mul (this) */
	Vec3 transformVector(const Vec3 & v, float * out_w = nullptr) const;

	/* (v) left mul (this) */
	Vec3 transformPoint(const Vec3 & v, float * out_w = nullptr) const;

	float determinant() const;

	bool decompose(Vec3 * out_scale, Quaternion * out_rotation, Vec3 * out_translation) const;

	Vec3 getUp() const;

	Vec3 getDown() const;

	Vec3 getLeft() const;

	Vec3 getRight() const;

	Vec3 getForward() const;

	Vec3 getBack() const;

	Matrix4 getNormalTransformMatrix() const;

	void output() const;

public:

	/*
          |+y
          |
          |		+x
          +--------
         /
        /+z
	*/

	/*
	createPerspectiveʹ����������ϵ(��һ����Ⱦ���������modelMatrix.transformPoint(vertices)�������صĶ���Ϊͬһ������ϵ)
	��createLookAtΪ��׼(������������)������һ��͸����׶�����
	fovy: �۾������ĽǶ�
	aspect: ��߱�ֵ(width / height)
	zNear�����ü������۾��ľ���
	zFar��Զ�ü������۾��ľ���
	*/
	static Matrix4 createPerspective(float fovy, float aspect, float zNear, float zFar);

	/*
	createLookAtʹ����������ϵ(��һ����Ⱦ���������modelMatrix.transformPoint(vertices)�������صĶ���Ϊͬһ������ϵ)
	eye: �۾������������λ�ã�Ĭ��Vec3(0, 0, 0)
	target: �۾����ŵ����������������λ�ã�Ĭ��Vec3(0, 0, -1)
	up: �۾��Ϸ����������Ĭ��Vec3(0, 1, 0)
	*/
	static Matrix4 createLookAt(const Vec3 & eye, const Vec3 & target, const Vec3 & up);

	/*
	createOrthoʹ����������ϵ(��һ����Ⱦ���������modelMatrix.transformPoint(vertices)�������صĶ���Ϊͬһ������ϵ)
	��createLookAtΪ��׼(������������)������һ��������׶�����
	ǰ4������Ϊ��׼�㵽��׶��8���������������Ӧ������4������
	zNear�����ü������۾��ľ���
	zFar��Զ�ü������۾��ľ���
	*/
	static Matrix4 createOrtho(float left, float right, float bottom, float top, float zNear, float zFar);

	static Matrix4 createTranslation(const Vec3 & translation);

	static Matrix4 createScale(float xs, float ys, float zs);

	static Matrix4 createRotate(const Vec3 & axis, float angle);

	static Matrix4 createRotateX(float angle);

	static Matrix4 createRotateY(float angle);

	static Matrix4 createRotateZ(float angle);

	static Matrix4 createRotate(const Quaternion & q);

	static Matrix4 createReflect(const Plane & plane);

private:
	/*
	column priority for opengl index :
	| 0   4   8   12|
	| 1   5   9   13|
	| 2   6   10  14|
	| 3   7   11  15|
	*/
	float mat[16];
};


FW_NS_END


#endif // !__LHSMT_MATRIX4_H__