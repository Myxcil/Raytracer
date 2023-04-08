#include "pch.h"
#include "InfinitePlane.h"

//----------------------------------------------------------------------------------------------------------------------------------------
InfinitePlane::InfinitePlane(const Vector3& _center, const Vector3& _normal, Material* _material) : 
	TraceableObject(_center, _material),
	normal(_normal)
{
}

//----------------------------------------------------------------------------------------------------------------------------------------
InfinitePlane::~InfinitePlane()
{
}

//----------------------------------------------------------------------------------------------------------------------------------------
void InfinitePlane::Raycast(HitInfo& _hitInfo, const Vector3& _rayOrigin, const Vector3& _rayDirection, Vector3::Type _tMin, Vector3::Type _tMax) const
{
	Vector3::Type denom = _rayDirection.Dot(normal);
	if (fabs(denom) > 1e-6)
	{
		Vector3 diff = center - _rayOrigin;
		Vector3::Type distance = diff.Dot(normal) / denom;
		if (distance > 1e-6)
		{
			_hitInfo.isHit = true;
			_hitInfo.distance = distance;
			
			_hitInfo.point = _rayOrigin + _rayDirection * distance;
			_hitInfo.surfaceNormal = normal;

			OnHit(_hitInfo);
		}
	}
}