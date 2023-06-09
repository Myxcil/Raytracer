#include "pch.h"
#include "TraceableObject.h"
#include "../Materials/Material.h"
#include "../HitInfo.h"

//----------------------------------------------------------------------------------------------------------------------------------------
TraceableObject::TraceableObject(const Vector3& _center, Material* _material) :
	center(_center),
	material(_material)
{
	material->AddRef();
}

//----------------------------------------------------------------------------------------------------------------------------------------
TraceableObject::~TraceableObject()
{
	material->Release();
}

//----------------------------------------------------------------------------------------------------------------------------------------
bool TraceableObject::Hit(const Ray& _ray, double _tMin, double _tMax) const
{
	return aabb.Hit(_ray, _tMin, _tMax);
}

//----------------------------------------------------------------------------------------------------------------------------------------
void TraceableObject::OnHit(HitInfo& _hitInfo) const
{
	_hitInfo.material = material;
	_hitInfo.object = this;
}

//----------------------------------------------------------------------------------------------------------------------------------------
bool TraceableObject::IsEmissive() const
{
	return material->IsImportant();
}