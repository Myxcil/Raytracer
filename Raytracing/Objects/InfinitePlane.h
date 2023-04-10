#pragma once
#include "TraceableObject.h"

//----------------------------------------------------------------------------------------------------------------------------------------
class InfinitePlane : public TraceableObject
{
public:
	//------------------------------------------------------------------------------------------------------------------------------------
	InfinitePlane(const Vector3& _center, const Vector3& _normal, bool _oneSided, Material* _material);
	~InfinitePlane();

	//------------------------------------------------------------------------------------------------------------------------------------
	void			Raycast(HitInfo& _hitInfo, const Ray& _ray, Vector3::Type _tMin, Vector3::Type _tMax) const override;

protected:
	//------------------------------------------------------------------------------------------------------------------------------------
	bool			oneSided;
	Vector3			normal;
	Vector3::Type	d;
};
