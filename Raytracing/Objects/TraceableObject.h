#pragma once

#include "AABB.h"
#include <vector>

//----------------------------------------------------------------------------------------------------------------------------------------
class TraceableObject;
class Material;

//----------------------------------------------------------------------------------------------------------------------------------------
struct HitInfo
{
	bool					isHit;
	double					distance;
	Vector3					point;
	Vector3					surfaceNormal;
	Vector3					uvw;
	bool					frontFace;
	const Material*			material;
	const TraceableObject*	object;

	//------------------------------------------------------------------------------------------------------------------------------------
	HitInfo() :
		isHit(false),
		distance(DBL_MAX)
	{
	}

	inline void SetNormal(const Vector3& _rayDirection, const Vector3& _normal)
	{
		frontFace = Vector3::Dot(_rayDirection, _normal) < 0;
		surfaceNormal = frontFace ? _normal : -_normal;
	}
};

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
