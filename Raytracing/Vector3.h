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
	union
	{
		struct
		{
			Type	x;
			Type	y;
			Type	z;
		};
		Type v[3];
	};

	//------------------------------------------------------------------------------------------------------------------------------------
	Vector3() { }
	Vector3(Type _x, Type _y, Type _z) : x(_x), y(_y), z(_z) { }

	//------------------------------------------------------------------------------------------------------------------------------------
	inline Vector3 operator-() const				{ return Vector3(-x, -y, -z); }

	inline Vector3& operator+=(const Vector3& _v)	{ x += _v.x; y += _v.y; z += _v.z; return *this; }
	inline Vector3& operator-=(const Vector3& _v)	{ x -= _v.x; y -= _v.y; z -= _v.z; return *this; }
	inline Vector3& operator*=(const Vector3& _v)	{ x *= _v.x; y *= _v.y; z *= _v.z; return *this; }
	inline Vector3& operator*=(Type _f)				{ x *= _f; y *= _f; z *= _f; return *this; }
	inline Vector3& operator/=(const Vector3& _v)	{ x /= _v.x; y /= _v.y; z /= _v.z; return *this; }
	inline Vector3& operator/=(Type _f)				{ x /= _f; y /= _f; z /= _f; return *this; }
	
	//------------------------------------------------------------------------------------------------------------------------------------
	inline Vector3 operator+(const Vector3& _v) const	{ return Vector3(*this).operator+=(_v); }
	inline Vector3 operator-(const Vector3& _v) const	{ return Vector3(*this).operator-=(_v); }
	inline Vector3 operator*(const Vector3& _v) const	{ return Vector3(*this).operator*=(_v); }
	inline Vector3 operator*(Vector3::Type _f) const	{ return Vector3(*this).operator*=(_f); }
	inline Vector3 operator/(const Vector3& _v) const	{ return Vector3(*this).operator/=(_v); }
	inline Vector3 operator/(Vector3::Type _f) const	{ return Vector3(*this).operator/=(_f); }

	friend Vector3 operator*(const Vector3::Type _f, const Vector3& _v) { return _v * _f; }
	
	//------------------------------------------------------------------------------------------------------------------------------------
	static Vector3		Div(const Vector3& _a, const Vector3& _b);

	//------------------------------------------------------------------------------------------------------------------------------------
	void				Normalize()								{ Type scale = Length(); if (scale > 0) this->operator*=(1.0/scale); }
	void				Saturate()								{ x = fmax(0,fmin(1,x)); y = fmax(0,fmin(1,y)); z = fmax(0,fmin(1,z)); }
	void				Floor()									{ x = floor(x); y = floor(y); z = floor(z); }
	void				Abs()									{ x = fabs(x); y = fabs(y); z = fabs(z); }
	Type				LengthSq() const						{ return x*x + y*y + z*z; }
	Type				Length() const							{ return sqrt(LengthSq()); }

	bool				NearZero() const						{ const Type s = 1e-8; return (fabs(x) < s) && (fabs(y) < s) && (fabs(z) < s); }

	static Type			Dot(const Vector3& _a, const Vector3& _b);
	static Vector3		Cross(const Vector3& _a, const Vector3& _b);
	static Vector3		Lerp(const Vector3& _a, const Vector3& _b, Vector3::Type _t);
	
	static Vector3		Reflect(const Vector3& _v, const Vector3& _n);
	static Vector3		Refract(const Vector3& _d, const Vector3& _n, Vector3::Type _eta_over_etaPrime);

	static Vector3		Min(const Vector3& _a, const Vector3& _b) { return Vector3(min(_a.x,_b.x),min(_a.y,_b.y),min(_a.z,_b.z)); }
	static Vector3		Max(const Vector3& _a, const Vector3& _b) { return Vector3(max(_a.x,_b.x),max(_a.y,_b.y),max(_a.z,_b.z)); }

	UINT32				ToRGB(Vector3::Type _scale) const;
};

//------------------------------------------------------------------------------------------------------------------------------------
using Color = Vector3;

//------------------------------------------------------------------------------------------------------------------------------------
struct Ray
{
	Vector3 origin;
	Vector3 direction;

	Ray() { }
	Ray(const Vector3& _origin, const Vector3& _direction) : origin(_origin), direction(_direction) { }

	Vector3 GetPoint(Vector3::Type _distance) const				{ return origin + _distance * direction; }
};

//------------------------------------------------------------------------------------------------------------------------------------
struct Helper
{
	//--------------------------------------------------------------------------------------------------------------------------------
	static void ConstructBasis(const Vector3& _direction, Vector3& _up, Vector3& _right)
	{
		_up = Vector3(0, 1, 0);
		if (fabs(Vector3::Dot(_up, _direction)) > 0.999)
		{
			_up = Vector3(0,0,1);
		}
		_right = Vector3::Cross(_up, _direction);
		_right.Normalize();
		_up = Vector3::Cross(_direction, _right);
	}


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
		if (Vector3::Dot(v, _normal) < 0)
		{
			v = -v;
		}
		return v;
	}

	//------------------------------------------------------------------------------------------------------------------------------------
	static int RandomInt(int min, int max)
	{
		return static_cast<int>(Random(min, max + 1));
	}

	//------------------------------------------------------------------------------------------------------------------------------------
	static Vector3 RandomCosineDirection()
	{
		Vector3::Type r1 = Random();
		Vector3::Type r2 = Random();
		Vector3::Type z = sqrt(1.0 - r2);
		
		Vector3::Type phi = 2.0 * M_PI * r1;
		Vector3::Type x = cos(phi) * sqrt(r2);
		Vector3::Type y = sin(phi) * sqrt(r2);

		return Vector3(x,y,z);
	}

	//------------------------------------------------------------------------------------------------------------------------------------
	static Vector3 RandomToSphere(Vector3::Type _radius, Vector3::Type _distanceSq)
	{
		Vector3::Type r1 = Random();
		Vector3::Type r2 = Random();
		Vector3::Type z = 1 + r2 * (sqrt(1.0 - _radius*_radius/_distanceSq) - 1);

		Vector3::Type phi = 2.0 * M_PI * r1;
		Vector3::Type x = cos(phi) * sqrt(1 - z*z);
		Vector3::Type y = sin(phi) * sqrt(1 - z*z);

		return Vector3(x,y,z);
	}

	//------------------------------------------------------------------------------------------------------------------------------------
	static void Log(const TCHAR* _szFormat, ...)
	{
		TCHAR szBuffer[1024];
		va_list arg;
		va_start(arg, _szFormat);
		_vsnwprintf_s(szBuffer, sizeof(szBuffer), _szFormat, arg);
		va_end(arg);
		OutputDebugString(szBuffer);
	}
};

//------------------------------------------------------------------------------------------------------------------------------------
struct ONB
{
	//------------------------------------------------------------------------------------------------------------------------------------
	Vector3 right, up, forward;
	
	//------------------------------------------------------------------------------------------------------------------------------------
	ONB() { }
	ONB(const Vector3& _direction) { Set(_direction); }

	//------------------------------------------------------------------------------------------------------------------------------------
	void Set(const Vector3& _direction)
	{
		forward = _direction;
		Helper::ConstructBasis(forward, up, right);
	}

	//------------------------------------------------------------------------------------------------------------------------------------
	Vector3 Transform(const Vector3& _p) const
	{
		return _p.x * right + _p.y * up + _p.z * forward;
	}
};
