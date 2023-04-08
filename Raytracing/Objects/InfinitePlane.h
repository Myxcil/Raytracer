#pragma once
#include "TraceableObject.h"

//----------------------------------------------------------------------------------------------------------------------------------------
class InfinitePlane : public TraceableObject
{
public:
	//------------------------------------------------------------------------------------------------------------------------------------
	InfinitePlane(const Vector3& _center, const Vector3& _normal, const Material& _material);
	~InfinitePlane();

	//------------------------------------------------------------------------------------------------------------------------------------
	void			Raycast(HitInfo& _hitInfo, const Vector3& _rayOrigin, const Vector3& _rayDirection, float _tMin, float _tMax) const override;

private:
	//------------------------------------------------------------------------------------------------------------------------------------
	Vector3			normal;
};
