#include "pch.h"
#include "TraceableObject.h"

//----------------------------------------------------------------------------------------------------------------------------------------
TraceableObject::TraceableObject(const Vector3& _center, const Material& _material) :
	center(_center),
	material(_material)
{
}

//----------------------------------------------------------------------------------------------------------------------------------------
TraceableObject::~TraceableObject()
{
}

//----------------------------------------------------------------------------------------------------------------------------------------
void TraceableObject::OnHit(HitInfo& _hitInfo) const
{
	_hitInfo.material = &material;
	_hitInfo.object = this;
}