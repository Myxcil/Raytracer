#pragma once

#define _USE_MATH_DEFINES
#include <cmath>
#include <random>

//----------------------------------------------------------------------------------------------------------------------------------------
struct Vector3
{
	//------------------------------------------------------------------------------------------------------------------------------------
	union
	{
		struct
		{
			double	x;
			double	y;
			double	z;
		};
		double v[3];
	};

	//------------------------------------------------------------------------------------------------------------------------------------
	Vector3() { }
	Vector3(double _x, double _y, double _z) : x(_x), y(_y), z(_z) { }

	//------------------------------------------------------------------------------------------------------------------------------------

	inline Vector3&		operator+=(const Vector3& _v)		{ x += _v.x; y += _v.y; z += _v.z; return *this; }
	inline Vector3&		operator-=(const Vector3& _v)		{ x -= _v.x; y -= _v.y; z -= _v.z; return *this; }
	inline Vector3&		operator*=(const Vector3& _v)		{ x *= _v.x; y *= _v.y; z *= _v.z; return *this; }
	inline Vector3&		operator*=(double _f)				{ x *= _f; y *= _f; z *= _f; return *this; }
	inline Vector3&		operator/=(const Vector3& _v)		{ x /= _v.x; y /= _v.y; z /= _v.z; return *this; }
	inline Vector3&		operator/=(double _f)				{ x /= _f; y /= _f; z /= _f; return *this; }
	
	//------------------------------------------------------------------------------------------------------------------------------------
	inline Vector3		operator-() const					{ return Vector3(-x, -y, -z); }

	inline Vector3		operator+(const Vector3& _v) const	{ return Vector3(*this).operator+=(_v); }
	inline Vector3		operator-(const Vector3& _v) const	{ return Vector3(*this).operator-=(_v); }
	inline Vector3		operator*(const Vector3& _v) const	{ return Vector3(*this).operator*=(_v); }
	inline Vector3		operator*(double _f) const			{ return Vector3(*this).operator*=(_f); }
	inline Vector3		operator/(const Vector3& _v) const	{ return Vector3(*this).operator/=(_v); }
	inline Vector3		operator/(double _f) const			{ return Vector3(*this).operator/=(_f); }

	friend Vector3		operator*(const double _f, const Vector3& _v) { return _v * _f; }
	
	//------------------------------------------------------------------------------------------------------------------------------------
	static Vector3		Div(const Vector3& _a, const Vector3& _b);

	//------------------------------------------------------------------------------------------------------------------------------------
	void				Normalize()								{ double scale = Length(); if (scale > 0) this->operator*=(1.0/scale); }
	void				Saturate()								{ x = fmax(0,fmin(1,x)); y = fmax(0,fmin(1,y)); z = fmax(0,fmin(1,z)); }
	void				Floor()									{ x = floor(x); y = floor(y); z = floor(z); }
	void				Abs()									{ x = fabs(x); y = fabs(y); z = fabs(z); }
	double				LengthSq() const						{ return x*x + y*y + z*z; }
	double				Length() const							{ return sqrt(LengthSq()); }

	bool				NearZero() const						{ const double s = 1e-8; return (fabs(x) < s) && (fabs(y) < s) && (fabs(z) < s); }

	static double		Dot(const Vector3& _a, const Vector3& _b)						{ return _a.x*_b.x + _a.y*_b.y + _a.z*_b.z; }
	static Vector3		Cross(const Vector3& _a, const Vector3& _b)						{ return Vector3(_a.y*_b.z - _a.z*_b.y, _a.z*_b.x - _a.x*_b.z, _a.x*_b.y - _a.y*_b.x); }
	static Vector3		Lerp(const Vector3& _a, const Vector3& _b, double _t)	{ return _a + _t * (_b - _a); }
	
	static Vector3		Reflect(const Vector3& _v, const Vector3& _n)					{ return _v - 2.0 * Dot(_v, _n) * _n; }
	static Vector3		Refract(const Vector3& _d, const Vector3& _n, double _eta_over_etaPrime);

	double				Min() const									{ return min(min(x, y), z); }
	double				Max() const									{ return max(max(x, y), z); }

	static Vector3		Min(const Vector3& _a, const Vector3& _b)	{ return Vector3(min(_a.x,_b.x),min(_a.y,_b.y),min(_a.z,_b.z)); }
	static Vector3		Max(const Vector3& _a, const Vector3& _b)	{ return Vector3(max(_a.x,_b.x),max(_a.y,_b.y),max(_a.z,_b.z)); }
	
	UINT32				ToRGB(double _scale) const;

	static void			ConstructBasis(const Vector3& _forward, Vector3& _right, Vector3& _up);
	Vector3				Transform(const Vector3& _xAxis, const Vector3& _yAxis, const Vector3& _zAxis) const;

	//------------------------------------------------------------------------------------------------------------------------------------
	static const Vector3	ZERO;
	static const Vector3	ONE;
	static const Vector3	HALF;
	static const Vector3	UNIT_X;
	static const Vector3	UNIT_Y;
	static const Vector3	UNIT_Z;
};
using Color = Vector3;

//----------------------------------------------------------------------------------------------------------------------------------------
struct Quaternion
{
	//------------------------------------------------------------------------------------------------------------------------------------
	union
	{
		struct
		{
			double x,y,z,w;
		};
		struct
		{
			Vector3	vector;
			double	scalar;
		};
		double v[4];
	};

	//------------------------------------------------------------------------------------------------------------------------------------
	Quaternion() { }
	Quaternion(double _x, double _y, double _z, double _w) : x(_x), y(_y), z(_z), w(_w) { }
	Quaternion(const Vector3& _axis, double _radians);
	Quaternion(const Vector3& _euler);
	Quaternion(const Vector3& _from, const Vector3& _to);

	//------------------------------------------------------------------------------------------------------------------------------------
	Vector3				Rotate(const Vector3& _v) const;
	Quaternion			Conjugate() const								{ return Quaternion(-x,-y,-z,w); }

	static double		Dot(const Quaternion& _a, const Quaternion& _b)	{ return _a.x*_b.x + _a.y*_b.y + _a.z*_b.z + _a.w*_b.w; }

	//------------------------------------------------------------------------------------------------------------------------------------
	 Quaternion			operator*(const Quaternion& _q) const;

	//------------------------------------------------------------------------------------------------------------------------------------
	static const Quaternion IDENTITY;
};

//------------------------------------------------------------------------------------------------------------------------------------
struct Ray
{
	Vector3 origin;
	Vector3 direction;

	Ray() { }
	Ray(const Vector3& _origin, const Vector3& _direction) : origin(_origin), direction(_direction) { }

	Vector3 GetPoint(double _distance) const				{ return origin + _distance * direction; }
};

//------------------------------------------------------------------------------------------------------------------------------------
struct Transfrom
{
	//------------------------------------------------------------------------------------------------------------------------------------
	union
	{
		Vector3	axis[3];
		struct
		{
			Vector3 xAxis;
			Vector3 yAxis;
			Vector3 zAxis;
		};
	};

	//------------------------------------------------------------------------------------------------------------------------------------
	Transfrom() { }
	Transfrom(const Vector3& _up) : yAxis(_up) { Vector3::ConstructBasis(yAxis, zAxis, xAxis); }
	Transfrom(const Vector3& _x, const Vector3& _y, const Vector3& _z) : xAxis(_x), yAxis(_y), zAxis(_z) { }

	//------------------------------------------------------------------------------------------------------------------------------------
	void	Init(const Vector3& _up) { yAxis = _up; Vector3::ConstructBasis(yAxis, zAxis, xAxis); }
	Vector3	Rotate(const Vector3& _v) const { return _v.Transform(xAxis, yAxis, zAxis); }

	//------------------------------------------------------------------------------------------------------------------------------------
	static const Transfrom IDENTITY;
};

//------------------------------------------------------------------------------------------------------------------------------------
struct Helper
{
	//------------------------------------------------------------------------------------------------------------------------------------
	// random value [0,1[
	static double Random()
	{
		static std::uniform_real_distribution<double> distribution(0.0, 1.0);
		static std::mt19937 generator;
		return distribution(generator);
	}

	//------------------------------------------------------------------------------------------------------------------------------------
	static double RandomGauss()
	{
		static std::normal_distribution<double> distribution;
		static std::mt19937 generator;
		return distribution(generator);
	}

	//------------------------------------------------------------------------------------------------------------------------------------
	// random value [min,max[
	static double Random(double _min, double _max)
	{
		return _min + (_max - _min) * Random();
	}

	//------------------------------------------------------------------------------------------------------------------------------------
	static Vector3 RandomDirection()
	{
		Vector3 v(Random(-1,1), Random(-1,1), Random(-1,1));
		v.Normalize();
		return v;
	}

	//------------------------------------------------------------------------------------------------------------------------------------
	static Vector3 RandomUnitSphere()
	{
		Vector3 v(RandomGauss(),RandomGauss(),RandomGauss());
		v.Normalize();
		return v;
	}

	//------------------------------------------------------------------------------------------------------------------------------------
	static Vector3 RandomDiscXZ()
	{
		double r = Random();
		double theta = Random(0, 2.0 * M_PI);
		return Vector3(r * cos(theta), 0, r * sin(theta));
	}

	//------------------------------------------------------------------------------------------------------------------------------------
	static Vector3 RandomDiscXZUniform()
	{
		double r = sqrt(Random());
		double theta = Random(0, 2.0 * M_PI);
		return Vector3(r * cos(theta), 0, r * sin(theta));
	}

	//------------------------------------------------------------------------------------------------------------------------------------
	static Vector3 RandomCosineHemisphere()
	{
		Vector3 v = RandomDiscXZ();
		v.y = sqrt(1.0 - v.x*v.x - v.z*v.z);
		return v;
	}

	//------------------------------------------------------------------------------------------------------------------------------------
	static void Log(const TCHAR* _szFormat, ...);
};
