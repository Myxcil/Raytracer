#pragma once

#define _USE_MATH_DEFINES
#include <cmath>
#include <random>

//----------------------------------------------------------------------------------------------------------------------------------------
struct Vector3
{
	//------------------------------------------------------------------------------------------------------------------------------------
	typedef double Type;

	//------------------------------------------------------------------------------------------------------------------------------------
	Type	x;
	Type	y;
	Type	z;

	//------------------------------------------------------------------------------------------------------------------------------------
	Vector3() :x(0), y(0), z(0) { }
	Vector3(Type _x, Type _y, Type _z) : x(_x), y(_y), z(_z) { }

	//------------------------------------------------------------------------------------------------------------------------------------
	inline Vector3& operator+=(const Vector3& _v)	{ x += _v.x; y += _v.y; z += _v.z; return *this; }
	inline Vector3& operator-=(const Vector3& _v)	{ x -= _v.x; y -= _v.y; z -= _v.z; return *this; }
	inline Vector3& operator*=(Type _f)				{ x *= _f; y *= _f; z *= _f; return *this; }
	inline Vector3& operator/=(Type _f)				{ return this->operator*=(1.0 / _f); }
	inline Vector3 operator-() const				{ return Vector3(-x, -y, -z); }

	//------------------------------------------------------------------------------------------------------------------------------------
	void	Normalize()								{ Type scale = 1.0 / Length(); x *= scale; y *= scale; z *= scale; }
	Type	LengthSq() const						{ return x*x + y*y + z*z; }
	Type	Length() const							{ return sqrt(LengthSq()); }

	Type	Dot(const Vector3& _v) const			{ return x*_v.x + y*_v.y + z*_v.z; }
	Vector3 Cross(const Vector3& _v) const			{ return Vector3(y*_v.z-z*_v.y, z*_v.x-x*_v.z, x*_v.y-y*_v.x); }
	Vector3	Lerp(const Vector3& _a, const Vector3& _b, Vector3::Type _t) { Type u = 1.0 - _t; x = u*_a.x + _t*_b.x; y = u*_a.y + _t*_b.y; z = u*_a.z + _t*_b.z;	return *this; }
};

//------------------------------------------------------------------------------------------------------------------------------------
inline Vector3 operator+(const Vector3& _a, const Vector3& _b)	{ return Vector3(_a).operator+=(_b); }
inline Vector3 operator-(const Vector3& _a, const Vector3& _b)	{ return Vector3(_a).operator-=(_b); }
inline Vector3 operator*(const Vector3& _a, Vector3::Type _f)	{ return Vector3(_a).operator*=(_f); }
inline Vector3 operator/(const Vector3& _a, Vector3::Type _f)	{ return Vector3(_a).operator/=(_f); }

inline Vector3 operator*(const float _f, const Vector3& _v) { return Vector3(_v).operator*=(_f); }

//------------------------------------------------------------------------------------------------------------------------------------
struct Helper
{
	//------------------------------------------------------------------------------------------------------------------------------------
	// random value [0,1[
	static Vector3::Type Random()
	{
		static std::uniform_real_distribution<Vector3::Type> distribution(0.0, 1.0);
		static std::mt19937 generator;
		return distribution(generator);
	}

	//------------------------------------------------------------------------------------------------------------------------------------
	// random value [min,max[
	static Vector3::Type Random(Vector3::Type _min, Vector3::Type _max)
	{
		return _min + (_max - _min) * Random();
	}

	//------------------------------------------------------------------------------------------------------------------------------------
	static Vector3 RandomUnitSphere()
	{
		Vector3 v(Random(-1,1), Random(-1,1), Random(-1,1));
		v.Normalize();
		return v;
	}

	//------------------------------------------------------------------------------------------------------------------------------------
	static Vector3 RandomUnitHemisphere(const Vector3& _normal)
	{
		Vector3 v = RandomUnitSphere();
		if (v.Dot(_normal) < 0)
		{
			v = -v;
		}
		return v;
	}
};