#include "pch.h"
#include "Math.h"

//------------------------------------------------------------------------------------------------------------------------------------
const Vector3	Vector3::ZERO(0,0,0);
const Vector3	Vector3::ONE(1,1,1);
const Vector3	Vector3::HALF(0.5,0.5,0.5);
const Vector3	Vector3::UNIT_X(1,0,0);
const Vector3	Vector3::UNIT_Y(0,1,0);
const Vector3	Vector3::UNIT_Z(0,0,1);

//----------------------------------------------------------------------------------------------------------------------------------------
Vector3 Vector3::Refract(const Vector3& _d, const Vector3& _n, double _eta_over_etaPrime)
{
	double cos_theta = fmin(Vector3::Dot(-_d, _n), 1.0);
	Vector3 perp = _eta_over_etaPrime * (_d + cos_theta * _n);
	Vector3 parallel = -sqrt(fabs(1.0 - perp.LengthSq())) * _n;
	return perp + parallel;
}

//----------------------------------------------------------------------------------------------------------------------------------------
#define TO_RGB(r,g,b) ((COLORREF)(((BYTE)(b)|((WORD)((BYTE)(g))<<8))|(((DWORD)(BYTE)(r))<<16)))

//----------------------------------------------------------------------------------------------------------------------------------------
UINT32 Vector3::ToRGB(double _scale) const
{
	double fr = (x != x) ? 0 : x;
	double fg = (y != y) ? 0 : y;
	double fb = (z != z) ? 0 : z;

	Vector3 scaled(sqrt(fr*_scale),sqrt(fg*_scale),sqrt(fb*_scale));
	scaled.Saturate();
	unsigned char r = static_cast<unsigned char>(scaled.x * 255.0f);
	unsigned char g = static_cast<unsigned char>(scaled.y * 255.0f);
	unsigned char b = static_cast<unsigned char>(scaled.z * 255.0f);
	return TO_RGB(r,g,b);
}

//----------------------------------------------------------------------------------------------------------------------------------------
Vector3 Vector3::Div(const Vector3& _a, const Vector3& _b)
{
	Vector3 v(_a);
	for (int i = 0; i < 3; ++i)
	{
		if (_b.v[i] != 0)
		{
			v.v[i] /= _b.v[i];
		}
		else if (v.v[i] != 0)
		{
			v.v[i] = v.v[i] >= 0 ? DBL_MAX : -DBL_MAX;
		}
	}
	return v;
}

//----------------------------------------------------------------------------------------------------------------------------------------
void Vector3::ConstructBasis(const Vector3& _a, Vector3& _b, Vector3& _c)
{
	_c = fabs(_a.y) < 0.99 ? Vector3::UNIT_Y : Vector3::UNIT_Z;
	_b = Vector3::Cross(_c, _a);
	_b.Normalize();
	_c = Vector3::Cross(_a, _b);
}

//----------------------------------------------------------------------------------------------------------------------------------------
Vector3 Vector3::Transform(const Vector3& _xAxis, const Vector3& _yAxis, const Vector3& _zAxis) const
{
	return x * _xAxis + y * _yAxis + z * _zAxis;
}

//----------------------------------------------------------------------------------------------------------------------------------------
void Helper::Log(const TCHAR* _szFormat, ...)
{
	static TCHAR szBuffer[1024];
	va_list arg;
	va_start(arg, _szFormat);
	_vsnwprintf_s(szBuffer, sizeof(szBuffer), _szFormat, arg);
	va_end(arg);
	OutputDebugString(szBuffer);
}

//----------------------------------------------------------------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------------------------------------------------------------
const Quaternion Quaternion::IDENTITY(0,0,0,1);

//----------------------------------------------------------------------------------------------------------------------------------------
static void FromAngleAxis(Quaternion& _q, const Vector3& _axis, double _radians)
{
	const double halfAngle = 0.5 * _radians;

	const double fcos = cos(halfAngle);
	const double fsin = sin(halfAngle);

	_q.x = _axis.x * fsin;
	_q.y = _axis.y * fsin;
	_q.z = _axis.z * fsin;
	_q.w = fcos;
}

//----------------------------------------------------------------------------------------------------------------------------------------
Quaternion::Quaternion(const Vector3& _axis, double _degree)
{
	FromAngleAxis(*this, _axis, _degree);
}

//----------------------------------------------------------------------------------------------------------------------------------------
Quaternion::Quaternion(const Vector3& _euler)
{
	const Vector3 halfAngle = _euler * 0.5;

	const double c1 = cos(halfAngle.y);
	const double s1 = sin(halfAngle.y);
	const double c2 = cos(halfAngle.x);
	const double s2 = sin(halfAngle.x);
	const double c3 = cos(halfAngle.z);
	const double s3 = sin(halfAngle.z);

	x = c1 * c2 * s3 + s1 * s2 * c3;
	y = s1 * c2 * c3 + c1 * s2 * s3;
	z = c1 * s2 * c3 - s1 * c2 * s3;

	w = c1 * c2 * c3 - s1 * s2 * s3;
}

//----------------------------------------------------------------------------------------------------------------------------------------
const double THRESHOLD = 0.99999;

//----------------------------------------------------------------------------------------------------------------------------------------
Quaternion::Quaternion(const Vector3& _from, const Vector3& _to)
{
	const double dot = Vector3::Dot(_from,_to);
	if (dot > THRESHOLD)
	{
		x = y = z = 0;
		w = 1;
	}
	else if (dot < -THRESHOLD)
	{
		x = y = w = 0;
		y = 1;
	}
	else
	{
		FromAngleAxis(*this, Vector3::Cross(_from,_to), acos(dot));
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------
Vector3 Quaternion::Rotate(const Vector3& _v) const
{
	Vector3 uv = Vector3::Cross(vector, _v);
	Vector3 uuv = Vector3::Cross(vector, uv);
	return _v + (2.0 * scalar * uv) + (2.0 * uuv);
}

//----------------------------------------------------------------------------------------------------------------------------------------
Quaternion Quaternion::operator*(const Quaternion& _q) const
{
	const double qx = (y * _q.z - _q.z * y) + x * _q.w + _q.x * w;
	const double qy = (z * _q.x - _q.x * z) + y * _q.w + _q.y * w;
	const double qz = (x * _q.y - _q.y * x) + z * _q.w + _q.z * w;
	const double qw = w * _q.w - (x * _q.x + y * _q.y + z * _q.z);
	return Quaternion(qx, qy, qz, qw);
}

//----------------------------------------------------------------------------------------------------------------------------------------
const Transfrom Transfrom::IDENTITY = Transfrom(Vector3::UNIT_X, Vector3::UNIT_Y, Vector3::UNIT_Z);