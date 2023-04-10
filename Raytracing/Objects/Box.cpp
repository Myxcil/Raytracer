#include "pch.h"
#include "Box.h"
#include "Quad.h"

//----------------------------------------------------------------------------------------------------------------------------------------
Box::Box(const Vector3& _center, const Vector3& _size, Material* _material) :
	TraceableObject(_center, _material),
	size(_size)
{
	aabb = AABB(center - size,center + size);
}

//----------------------------------------------------------------------------------------------------------------------------------------
void Box::Raycast(HitInfo& _hitInfo, const Ray& _ray, Vector3::Type _tMin, Vector3::Type _tMax) const
{
	aabb.Raycast(_hitInfo, _ray, _tMin, _tMax);
	if (_hitInfo.isHit)
	{
		Vector3 local = (_hitInfo.point - center) / size;
		Vector3 normal = _hitInfo.surfaceNormal;
		if (fabs(normal.x) > fabs(normal.y) && fabs(normal.x) > fabs(normal.z))
		{
			_hitInfo.uvw = Vector3(local.y, local.z, 0);
		}
		else if (fabs(normal.y) > fabs(normal.x) && fabs(normal.y) > fabs(normal.z))
		{
			_hitInfo.uvw = Vector3(local.x, local.z, 0);
		}
		else
		{
			_hitInfo.uvw = Vector3(local.x, local.y, 0);
		}

		OnHit(_hitInfo);
	}
}
