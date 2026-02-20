
#include "GMath.h"
#include <cfloat>

const XMFLOAT2 ZeroVec2d(0.0f, 0.0f);
const XMFLOAT2 Ones2d(1.0f, 1.0f);
const XMFLOAT2 UnitVecX2d(1.0f, 0.0f);
const XMFLOAT2 UnitVecY2d(0.0f, 1.0f);

const XMFLOAT3 ZeroVec3d(0.0f, 0.0f, 0.0f);
const XMFLOAT3 Ones3d(1.0f, 1.0f, 1.0f);
const XMFLOAT3 UnitVecX3d(1.0f, 0.0f, 0.0f);
const XMFLOAT3 UnitVecY3d(0.0f, 1.0f, 0.0f);
const XMFLOAT3 UnitVecZ3d(0.0f, 0.0f, 1.0f);

XMFLOAT2 operator +(const XMFLOAT2& a, const XMFLOAT2& b)
{
	return XMFLOAT2(a.x + b.x, a.y + b.y);
}

XMFLOAT2 operator -(const XMFLOAT2& a, const XMFLOAT2& b)
{
	return XMFLOAT2(a.x - b.x, a.y - b.y);
}

XMFLOAT2 operator *(const XMFLOAT2& a, float b)
{
	return XMFLOAT2(a.x * b, a.y * b);
}

XMFLOAT2 operator *(float a, const XMFLOAT2& b)
{
	return XMFLOAT2(a * b.x, a * b.y);
}

XMFLOAT2 operator *(const XMFLOAT2& a, const XMFLOAT2& b)
{
	return XMFLOAT2(a.x * b.x, a.y * b.y);
}

void operator +=(XMFLOAT2& a, const XMFLOAT2& b)
{
	a.x += b.x;
	a.y += b.y;
}

void operator -=(XMFLOAT2& a, const XMFLOAT2& b)
{
	a.x -= b.x;
	a.y -= b.y;
}

void operator *=(XMFLOAT2& a, float b)
{
	a.x *= b;
	a.y *= b;
}

float length(const XMFLOAT2& v)
{
	return std::sqrt(v.x * v.x + v.y * v.y);
}

float lengthSq(const XMFLOAT2& v)
{
	return v.x * v.x + v.y * v.y;
}

XMFLOAT2 normalize(const XMFLOAT2& v)
{
	float len = length(v);
	if (len < FLT_EPSILON) return XMFLOAT2(0.0f, 0.0f);

	float inLen = 1.0f / len;
	return inLen * v;
}

XMFLOAT2 unitVecAngle(float theta)
{
	return XMFLOAT2(std::cos(theta), std::sin(theta));
}

float dot(const XMFLOAT2& a, const XMFLOAT2& b)
{
	return a.x * b.x + a.y * b.y;
}

float cross(const XMFLOAT2& a, const XMFLOAT2& b)
{
	return a.x * b.y - a.y * b.x;
}

XMFLOAT3 operator+ (const XMFLOAT3& a, const XMFLOAT3& b)
{
	return XMFLOAT3(a.x + b.x, a.y + b.y, a.z + b.z);
}

XMFLOAT3 operator- (const XMFLOAT3& a, const XMFLOAT3& b)
{
	return XMFLOAT3(a.x - b.x, a.y - b.y, a.z - b.z);
}

XMFLOAT3 operator* (const XMFLOAT3& a, float b)
{
	return XMFLOAT3(a.x * b, a.y * b, a.z * b);
}

XMFLOAT3 operator* (float a, const XMFLOAT3& b)
{
	return XMFLOAT3(a * b.x, a * b.y, a * b.z);
}

XMFLOAT3 operator* (const XMFLOAT3& a, const XMFLOAT3& b)
{
	return XMFLOAT3(a.x * b.x, a.y * b.y, a.z * b.z);
}

void operator+= (XMFLOAT3& a, const XMFLOAT3& b)
{
	a.x += b.x;
	a.y += b.y;
	a.z += b.z;
}

void operator-= (XMFLOAT3& a, const XMFLOAT3& b)
{
	a.x -= b.x;
	a.y -= b.y;
	a.z -= b.z;
}

void operator*= (XMFLOAT3& a, float b)
{
	a.x *= b;
	a.y *= b;
	a.z *= b;
}

void operator *=(XMFLOAT3& a, const XMFLOAT3& b)
{
	a.x *= b.x;
	a.y *= b.y;
	a.z *= b.z;
}

float length(const XMFLOAT3& v)
{
	return std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
}

float lengthSq(const XMFLOAT3& v)
{
	return v.x * v.x + v.y * v.y + v.z * v.z;
}

XMFLOAT3 normalize(const XMFLOAT3& v)
{
	float len = length(v);
	if (len < FLT_EPSILON) return ZeroVec3d;

	float inLen = 1.0f / len;
	return inLen * v;
}

float dot(const XMFLOAT3& a, const XMFLOAT3& b)
{
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

XMFLOAT3 cross(const XMFLOAT3& a, const XMFLOAT3& b)
{
	return XMFLOAT3(
		a.y * b.z - a.z * b.y,
		a.z * b.x - a.x * b.z,
		a.x * b.y - a.y * b.x);
}

XMFLOAT3 unitVecAngleX(float theta)
{
	return XMFLOAT3(0.0f, XMScalarCos(theta), XMScalarSin(theta));
}

XMFLOAT3 unitVecAngleY(float theta)
{
	return XMFLOAT3(XMScalarSin(theta), 0.0f, XMScalarCos(theta));
}

XMFLOAT3 unitVecAngleZ(float theta)
{
	return XMFLOAT3(XMScalarCos(theta), XMScalarSin(theta), 0.0f);
}

XMFLOAT3 mul(const XMFLOAT3& vec, const XMMATRIX& mat, float w, bool wdiv)
{
	XMVECTOR p = XMVectorSet(vec.x, vec.y, vec.z, w);
	XMVECTOR q = XMVector4Transform(p, mat);

	float d = (wdiv) ? 1.0f / q.m128_f32[3] : 1.0f;

	return d * XMFLOAT3(q.m128_f32[0], q.m128_f32[1], q.m128_f32[2]);
}

float calcLengthProjAxis(const XMFLOAT3& axis,
	const XMFLOAT3& bx, const XMFLOAT3& by, const XMFLOAT3& bz)
{
	float s1 = std::fabs(dot(axis, bx));
	float s2 = std::fabs(dot(axis, by));
	float s3 = std::fabs(dot(axis, bz));

	return s1 + s2 + s3;
}


