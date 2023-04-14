#include "pch.h"
#include "Math.h"

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
	Vector3 scaled(sqrt(x*_scale),sqrt(y*_scale),sqrt(z*_scale));
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
void Vector3::ConstructBasis(const Vector3& _forward, Vector3& _right, Vector3& _up)
{
	_up = Vector3(0,1,0);
	if (fabs(Dot(_up, _forward)) > 0.99)
	{
		_up = Vector3(0,0,1);
	}
	_right = Vector3::Cross(_up, _forward);
	_right.Normalize();
	_up = Vector3::Cross(_forward, _right);
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
static void FromAngleAxis(Quaternion& _q, const Vector3& _axis, double _degree)
{
	const double halfAngle = 0.5 * _degree * M_PI / 180.0;

	const double fcos = cos(halfAngle);
	const double fsin = sin(halfAngle);

	_q.v[0] = _axis.v[0] * fsin;
	_q.v[1] = _axis.v[1] * fsin;
	_q.v[2] = _axis.v[2] * fsin;
	_q.v[3] = fcos;
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
	Quaternion qv = Quaternion(_v.x, _v.y, _v.z, 1.0);
	Quaternion r = *this * qv * Conjugate();
	return Vector3(r.x, r.y, r.z);
}

//----------------------------------------------------------------------------------------------------------------------------------------
Quaternion& Quaternion::operator*=(const Quaternion& _q)
{
	const double qx = (v[1] * _q.v[2] - v[2] * _q.v[1]) + v[0] * _q.v[3] + _q.v[0] * v[3];
	const double qy = (v[2] * _q.v[0] - v[0] * _q.v[2]) + v[1] * _q.v[3] + _q.v[1] * v[3];
	const double qz = (v[0] * _q.v[1] - v[1] * _q.v[0]) + v[2] * _q.v[3] + _q.v[2] * v[3];
	const double qw = v[3] * _q.v[3] - (v[0] * _q.v[0] + v[1] * _q.v[1] + v[2] * _q.v[2]);
	v[0] = qx;
	v[1] = qy;
	v[2] = qz;
	v[3] = qw;
	return *this;
}
