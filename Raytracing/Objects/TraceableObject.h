#pragma once

#include "AABB.h"
#include <vector>

//----------------------------------------------------------------------------------------------------------------------------------------
class Material;

//----------------------------------------------------------------------------------------------------------------------------------------
class TraceableObject
{
public:
	//------------------------------------------------------------------------------------------------------------------------------------
	virtual ~TraceableObject();

	//------------------------------------------------------------------------------------------------------------------------------------
	bool			Hit(const Ray& _ray, double _tMin, double _tMax) const;
	virtual void	Raycast(HitInfo& _hitInfo, const Ray& _ray, double _tMin, double _tMax) const = 0;
	
	//------------------------------------------------------------------------------------------------------------------------------------
	const AABB&		GetAABB() const { return aabb; }
	bool			IsEmissive() const;

	//------------------------------------------------------------------------------------------------------------------------------------
	virtual double	PDFValue(const HitInfo& _hitInfo) const		{ return 0; }
	virtual Vector3	PDFGenerate(const HitInfo& _hitInfo) const	{ return Vector3::UNIT_X; }

protected:
	//------------------------------------------------------------------------------------------------------------------------------------
	TraceableObject(const Vector3& _center, Material* _material);
	virtual void	OnHit(HitInfo& _hitInfo) const;

protected:
	//------------------------------------------------------------------------------------------------------------------------------------
	Vector3			center;
	AABB			aabb;

private:
	//------------------------------------------------------------------------------------------------------------------------------------
	Material*		material;
};

typedef std::vector<TraceableObject*> TraceableObjects;
