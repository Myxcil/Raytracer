#pragma once

//----------------------------------------------------------------------------------------------------------------------------------------
struct HitInfo;

//----------------------------------------------------------------------------------------------------------------------------------------
class AABB
{
public:
	//------------------------------------------------------------------------------------------------------------------------------------
	AABB() : vMin(0,0,0),vMax(0,0,0),isValid(false) { }
	AABB(const Vector3& _min, const Vector3& _max) : vMin(_min), vMax(_max),isValid(true) { }

	//------------------------------------------------------------------------------------------------------------------------------------
	bool			IsValid() const { return isValid; }

	//------------------------------------------------------------------------------------------------------------------------------------
	void			Merge(const Vector3& _p);
	void			Merge(const AABB& _other);
	bool			Hit(const Ray& _ray, Vector3::Type _tMin, Vector3::Type _tMax) const;
	void			Raycast(HitInfo& _hitInfo, const Ray& _ray, Vector3::Type _tMin, Vector3::Type _tMax) const;

	//------------------------------------------------------------------------------------------------------------------------------------
	const Vector3&	Min() const { return vMin; }
	const Vector3&	Max() const { return vMax; }
	Vector3			Size() const { return vMax - vMin; }
	void			EnsureMinSize(Vector3::Type _minSize);

	//------------------------------------------------------------------------------------------------------------------------------------
	static AABB		Merge(const AABB& _a, const AABB& _b)	{ AABB merged(_a); merged.Merge(_b); return merged; }

private:
	//------------------------------------------------------------------------------------------------------------------------------------
	bool			isValid;
	Vector3			vMin;
	Vector3			vMax;
};