#include "pch.h"
#include "InfinitePlane.h"
#include "../HitInfo.h"

//----------------------------------------------------------------------------------------------------------------------------------------
InfinitePlane::InfinitePlane(const Vector3& _center, const Vector3& _normal, bool _oneSided, Material* _material) :
	TraceableObject(_center, _material),
	oneSided(_oneSided),
	normal(_normal)
{
	d = -Vector3::Dot(_center, normal);
}

//----------------------------------------------------------------------------------------------------------------------------------------
InfinitePlane::~InfinitePlane()
{
}

//----------------------------------------------------------------------------------------------------------------------------------------
void InfinitePlane::Raycast(HitInfo& _hitInfo, const Ray& _ray, double _tMin, double _tMax) const
{
	double denom = Vector3::Dot(_ray.direction, normal);
	if (!oneSided)
	{
		denom = fabs(denom);
	}
	if (denom < 1e-6)
	{
		Vector3 diff = center - _ray.origin;
		double distance = fabs(Vector3::Dot(diff, normal) / denom);
		if (distance > 1e-6)
		{
			_hitInfo.isHit = true;
			_hitInfo.distance = distance;
			
			_hitInfo.point = _ray.origin + _ray.direction * distance;
			_hitInfo.SetNormal(_ray.direction, normal);

			_hitInfo.uvw = _hitInfo.point;

			OnHit(_hitInfo);
		}
	}
}
