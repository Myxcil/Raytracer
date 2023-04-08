#pragma once

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
	const Material*			material;
	const TraceableObject*	object;

	//------------------------------------------------------------------------------------------------------------------------------------
	HitInfo() :
		isHit(false),
		distance(DBL_MAX),
		point(0,0,0),
		surfaceNormal(0,1,0),
		material(nullptr),
		object(nullptr)
	{
	}
};

//----------------------------------------------------------------------------------------------------------------------------------------
class TraceableObject
{
public:
	//------------------------------------------------------------------------------------------------------------------------------------
	virtual ~TraceableObject();
	virtual void	Raycast(HitInfo& _hitInfo, const Vector3& _rayOrigin, const Vector3& _rayDirection, Vector3::Type _tMin, Vector3::Type _tMax) const = 0;

protected:
	//------------------------------------------------------------------------------------------------------------------------------------
	TraceableObject(const Vector3& _center, Material* _material);
	virtual void	OnHit(HitInfo& _hitInfo) const;

protected:
	//------------------------------------------------------------------------------------------------------------------------------------
	Vector3			center;

private:
	//------------------------------------------------------------------------------------------------------------------------------------
	Material*		material;
};
