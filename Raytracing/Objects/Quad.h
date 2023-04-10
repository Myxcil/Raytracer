#pragma once

#include "InfinitePlane.h"

//----------------------------------------------------------------------------------------------------------------------------------------
class Quad : public InfinitePlane
{
public:
	//------------------------------------------------------------------------------------------------------------------------------------
	Quad(const Vector3& _center, const Vector3& _normal, const Vector3& _size, Material* _material);

	//------------------------------------------------------------------------------------------------------------------------------------
	void			Raycast(HitInfo& _hitInfo, const Ray& _ray, Vector3::Type _tMin, Vector3::Type _tMax) const override;

	//------------------------------------------------------------------------------------------------------------------------------------
	Vector3::Type	CalcPDFValue(const Vector3& _origin, const Vector3& _direction) const override;
	Vector3			GetRandomDirection(const Vector3& _origin) const override;

private:
	//------------------------------------------------------------------------------------------------------------------------------------
	void			ConstructAABB();

private:
	//------------------------------------------------------------------------------------------------------------------------------------
	Vector3			size;
	Vector3			u,v;
};