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

	double			PDFValue(const HitInfo& _hitInfo) const;
	Vector3			PDFGenerate(const HitInfo& _hitInfo) const;

private:
	//------------------------------------------------------------------------------------------------------------------------------------
	static bool		HitSphere(const Ray& _ray, const Vector3& _center, double _radius, double& _t0, double& _t1);
	static void		CalcSphereUV(const Vector3& _point, Vector3& _uvw);

private:
	//------------------------------------------------------------------------------------------------------------------------------------
	double	radius;
};
