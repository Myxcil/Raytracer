#pragma once
#include "TraceableObject.h"

//----------------------------------------------------------------------------------------------------------------------------------------
class InfinitePlane : public TraceableObject
{
public:
	//------------------------------------------------------------------------------------------------------------------------------------
	InfinitePlane(const Vector3& _center, const Vector3& _normal, Material* _material);
	~InfinitePlane();

	//------------------------------------------------------------------------------------------------------------------------------------
	void			Raycast(HitInfo& _hitInfo, const Vector3& _rayOrigin, const Vector3& _rayDirection, Vector3::Type _tMin, Vector3::Type _tMax) const override;

private:
	//------------------------------------------------------------------------------------------------------------------------------------
	Vector3			normal;
};
