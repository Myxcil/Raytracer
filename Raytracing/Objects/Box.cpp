#include "pch.h"
#include "Box.h"
#include "Quad.h"

//----------------------------------------------------------------------------------------------------------------------------------------
Box::Box(const Vector3& _center, const Vector3& _size, Material* _material) :
	TraceableObject(_center, _material),
	size(_size)
{
	aabb = AABB(center - size,center + size);
	ZeroMemory(quads,sizeof(quads));
}

//----------------------------------------------------------------------------------------------------------------------------------------
void Box::Raycast(HitInfo& _hitInfo, const Ray& _ray, Vector3::Type _tMin, Vector3::Type _tMax) const
{
	for (int i = 0; i < 6; ++i)
	{
		HitInfo objHit;
		quads[i]->Raycast(objHit, _ray, _tMin, _tMax);
		if (objHit.isHit && objHit.distance < _hitInfo.distance)
		{
			_hitInfo = objHit;
		}
	}
}
