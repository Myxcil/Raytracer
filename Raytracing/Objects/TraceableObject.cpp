#include "pch.h"
#include "TraceableObject.h"
#include "../Material.h"

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
void TraceableObject::OnHit(HitInfo& _hitInfo) const
{
	_hitInfo.material = material;
	_hitInfo.object = this;
}