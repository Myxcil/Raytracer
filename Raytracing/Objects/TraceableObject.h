#pragma once

#include "AABB.h"

//----------------------------------------------------------------------------------------------------------------------------------------
class TraceableObject;
class Material;

//----------------------------------------------------------------------------------------------------------------------------------------
struct HitInfo
{
	bool					isHit;
	Vector3::Type			distance;
	Vector3					point;
	Vector3					surfaceNormal;
	Vector3					uvw;
	bool					frontFace;
	const Material*			material;
	const TraceableObject*	object;

	//------------------------------------------------------------------------------------------------------------------------------------
	HitInfo() :
		isHit(false),
		distance(DBL_MAX),
		point(0,0,0),
		surfaceNormal(0,1,0),
		uvw(0,0,0),
		frontFace(false),
		material(nullptr),
		object(nullptr)
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

	bool			Hit(const Ray& _ray, Vector3::Type _tMin, Vector3::Type _tMax) const;
	virtual void	Raycast(HitInfo& _hitInfo, const Ray& _ray, Vector3::Type _tMin, Vector3::Type _tMax) const = 0;

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
