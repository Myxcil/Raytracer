#pragma once

#include "TraceableObject.h"

//----------------------------------------------------------------------------------------------------------------------------------------
class Sphere : public TraceableObject
{
public:
	//------------------------------------------------------------------------------------------------------------------------------------
	Sphere(const Vector3& _center, double _radius, Material* _material);
	~Sphere();

	//------------------------------------------------------------------------------------------------------------------------------------
	void			Raycast(HitInfo& _hitInfo, const Ray& _ray, double _tMin, double _tMax) const override;
	
	static bool		Intersect(const Ray& _ray, const Vector3& _center, double _radius, double& _t0, double& _t1);

private:
	//------------------------------------------------------------------------------------------------------------------------------------
	static void		CalcSphereUV(const Vector3& _point, Vector3& _uvw);

private:
	//------------------------------------------------------------------------------------------------------------------------------------
	double	radius;
};
