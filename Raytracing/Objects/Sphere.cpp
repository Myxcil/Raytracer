#include "pch.h"
#include "Sphere.h"

//----------------------------------------------------------------------------------------------------------------------------------------
Sphere::Sphere(const Vector3& _center, Vector3::Type _radius, Material* _material) : TraceableObject(_center, _material)
{
	radius = _radius;
}

//----------------------------------------------------------------------------------------------------------------------------------------
Sphere::~Sphere()
{
}

//----------------------------------------------------------------------------------------------------------------------------------------
//		x² + y² + z² = r²
// =>	|ray.pos + ray.dir * dst|² = r²
//----------------------------------------------------------------------------------------------------------------------------------------
void Sphere::Raycast(HitInfo& _hitInfo, const Vector3& _rayOrigin, const Vector3& _rayDirection, Vector3::Type _tMin, Vector3::Type _tMax) const
{
	_hitInfo.isHit = false;

	Vector3 offsetRayOrigin = _rayOrigin - center;

	// coefficients for quadratic equation
	Vector3::Type a = _rayDirection.LengthSq();
	Vector3::Type half_b = offsetRayOrigin.Dot(_rayDirection);
	Vector3::Type c = offsetRayOrigin.LengthSq() - radius * radius;

	// quadratic discrinant
	Vector3::Type d = half_b * half_b - a * c;
	if (d >= 0)
	{
		Vector3::Type sqrt_d = sqrt(d);

		Vector3::Type distance = (-half_b - sqrt_d) / a;
		if (distance < _tMin || distance > _tMax)
		{
			distance = (-half_b + sqrt_d) / a;
		}
		if (_tMin <= distance && distance <= _tMax)
		{
			_hitInfo.isHit = true;
			_hitInfo.distance = distance;

			_hitInfo.point = _rayOrigin + _rayDirection * distance;
			_hitInfo.surfaceNormal = _hitInfo.point - center;
			_hitInfo.surfaceNormal.Normalize();

			OnHit(_hitInfo);
		}
	}
}
