#include "pch.h"
#include "InfinitePlane.h"

//----------------------------------------------------------------------------------------------------------------------------------------
InfinitePlane::InfinitePlane(const Vector3& _center, const Vector3& _normal, const Material& _material) : 
	TraceableObject(_center, _material),
	normal(_normal)
{
}

//----------------------------------------------------------------------------------------------------------------------------------------
InfinitePlane::~InfinitePlane()
{
}

//----------------------------------------------------------------------------------------------------------------------------------------
void InfinitePlane::Raycast(HitInfo& _hitInfo, const Vector3& _rayOrigin, const Vector3& _rayDirection, float _tMin, float _tMax) const
{
	float denom = _rayDirection.Dot(normal);
	if (FastMath::Abs(denom) > 1e-6f)
	{
		Vector3 diff = center - _rayOrigin;
		float distance = diff.Dot(normal) / denom;
		if (distance > 1e-6f)
		{
			_hitInfo.isHit = true;
			_hitInfo.distance = distance;
			
			_hitInfo.point = _rayOrigin + _rayDirection * distance;
			_hitInfo.surfaceNormal = normal;

			OnHit(_hitInfo);
		}
	}
}