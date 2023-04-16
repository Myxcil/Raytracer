#pragma once

#include "InfinitePlane.h"

//----------------------------------------------------------------------------------------------------------------------------------------
class Quad : public InfinitePlane
{
public:
	//------------------------------------------------------------------------------------------------------------------------------------
	Quad(const Vector3& _center, const Vector3& _normal, const Vector3& _size, Material* _material);

	//------------------------------------------------------------------------------------------------------------------------------------
	void			Raycast(HitInfo& _hitInfo, const Ray& _ray, double _tMin, double _tMax) const override;

	double			PDFValue(const HitInfo& _hitInfo) const override;
	Vector3			PDFGenerate(const HitInfo& _hitInfo) const override;

private:
	//------------------------------------------------------------------------------------------------------------------------------------
	void			ConstructAABB();

private:
	//------------------------------------------------------------------------------------------------------------------------------------
	Vector3			size;
	Vector3			u,v;
	double			area;
};