#include "pch.h"
#include "Box.h"

//----------------------------------------------------------------------------------------------------------------------------------------
Box::Box(const Vector3& _center, const Vector3& _size, Material* _material) :
	TraceableObject(_center, _material),
	size(_size)
{
}

//----------------------------------------------------------------------------------------------------------------------------------------
void Box::Raycast(HitInfo& _hitInfo, const Ray& _ray, Vector3::Type _tMin, Vector3::Type _tMax) const
{
}
