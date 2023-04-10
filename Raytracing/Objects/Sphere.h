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
	void			Raycast(HitInfo& _hitInfo, const Ray& _ray, Vector3::Type _tMin, Vector3::Type _tMax) const override;
	Vector3::Type	CalcPDFValue(const Vector3& _origin, const Vector3& _direction) const override;
	Vector3			GetRandomDirection(const Vector3& _origin) const override;

private:
	//------------------------------------------------------------------------------------------------------------------------------------
	static bool		Intersect(const Ray& _ray, const Vector3& _center, Vector3::Type _radius, Vector3::Type& _t0, Vector3::Type& _t1);
	static void		CalcSphereUV(const Vector3& _point, Vector3& _uvw);

private:
	//------------------------------------------------------------------------------------------------------------------------------------
	Vector3::Type	radius;
};
