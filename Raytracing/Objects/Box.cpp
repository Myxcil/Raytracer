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
		OnHit(_hitInfo);
	}
}
