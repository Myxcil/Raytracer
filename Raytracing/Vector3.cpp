#include "pch.h"
#include "Vector3.h"

//----------------------------------------------------------------------------------------------------------------------------------------
Vector3 Vector3::Refract(const Vector3& _d, const Vector3& _n, Vector3::Type _eta_over_etaPrime)
{
	Vector3::Type cos_theta = fmin(Vector3::Dot(-_d, _n), 1.0);
	Vector3 perp = _eta_over_etaPrime * (_d + cos_theta * _n);
	Vector3 parallel = -sqrt(fabs(1.0 - perp.LengthSq())) * _n;
	return perp + parallel;
}

//----------------------------------------------------------------------------------------------------------------------------------------
#define TO_RGB(r,g,b) ((COLORREF)(((BYTE)(b)|((WORD)((BYTE)(g))<<8))|(((DWORD)(BYTE)(r))<<16)))

//----------------------------------------------------------------------------------------------------------------------------------------
UINT32 Vector3::ToRGB(Vector3::Type _scale) const
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