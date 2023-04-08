#pragma once

using namespace DirectX::SimpleMath;

//----------------------------------------------------------------------------------------------------------------------------------------
class TraceableObject;

//----------------------------------------------------------------------------------------------------------------------------------------
struct Material
{
	Vector3			color;
	Vector3			emissive;

	Material(const Vector3& _color, const Vector3& _emissive = Vector3(0,0,0))
	{
		color = _color;
		emissive = _emissive;
	}
};

//----------------------------------------------------------------------------------------------------------------------------------------
struct HitInfo
{
	bool					isHit;
	float					distance;
	Vector3					point;
	Vector3					surfaceNormal;
	const Material*			material;
	const TraceableObject*	object;

	HitInfo() :
		isHit(false),
		distance(FLT_MAX),
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
	virtual void	Raycast(HitInfo& _hitInfo, const Vector3& _rayOrigin, const Vector3& _rayDirection, float _tMin, float _tMax) const = 0;

protected:
	//------------------------------------------------------------------------------------------------------------------------------------
	TraceableObject(const Vector3& _center, const Material& _material);
	virtual void	OnHit(HitInfo& _hitInfo) const;

protected:
	//------------------------------------------------------------------------------------------------------------------------------------
	Vector3			center;

private:
	//------------------------------------------------------------------------------------------------------------------------------------
	const Material	material;
};
