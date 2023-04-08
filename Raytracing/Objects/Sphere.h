#pragma once

#include "TraceableObject.h"

//----------------------------------------------------------------------------------------------------------------------------------------
class Sphere : public TraceableObject
{
public:
	//------------------------------------------------------------------------------------------------------------------------------------
	Sphere(const Vector3& _center, Vector3::Type _radius, Material* _material);
	~Sphere();

	//------------------------------------------------------------------------------------------------------------------------------------
	void			Raycast(HitInfo& _hitInfo, const Vector3& _rayOrigin, const Vector3& _rayDirection, Vector3::Type _tMin, Vector3::Type _tMax) const override;

private:
	//------------------------------------------------------------------------------------------------------------------------------------
	Vector3::Type	radius;
};
