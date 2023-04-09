#include "pch.h"
#include "AABB.h"

//----------------------------------------------------------------------------------------------------------------------------------------
void AABB::Merge(const Vector3& _p)
{
	vMin.x = fmin(vMin.x, _p.x);
	vMin.y = fmin(vMin.y, _p.y);
	vMin.z = fmin(vMin.z, _p.z);

	vMax.x = fmax(vMax.x, _p.x);
	vMax.y = fmax(vMax.y, _p.y);
	vMax.z = fmax(vMax.z, _p.z);
}

//----------------------------------------------------------------------------------------------------------------------------------------
void AABB::Merge(const AABB& _other)
{
	vMin.x = fmin(vMin.x, _other.vMin.x);
	vMin.y = fmin(vMin.y, _other.vMin.y);
	vMin.z = fmin(vMin.z, _other.vMin.z);

	vMax.x = fmax(vMax.x, _other.vMax.x);
	vMax.y = fmax(vMax.y, _other.vMax.y);
	vMax.z = fmax(vMax.z, _other.vMax.z);
}

//----------------------------------------------------------------------------------------------------------------------------------------
bool AABB::Hit(const Ray& _ray, Vector3::Type _tMin, Vector3::Type _tMax) const
{
	if (isValid)
	{
		Vector3 t = Vector3(0,0,0);
		for (int i = 0; i < 3; ++i)
		{
			Vector3::Type invD = 1.0 / _ray.direction.v[i];
			Vector3::Type t0 = (vMin.v[i] - _ray.origin.v[i]) * invD;
			Vector3::Type t1 = (vMax.v[i] - _ray.origin.v[i]) * invD;
			if (invD < 0)
			{
				std::swap(t0,t1);
			}
			_tMin = t0 > _tMin ? t0 : _tMin;
			_tMax = t1 < _tMin ? t1 : _tMax;
			if (_tMax <= _tMin)
				return false;

			t.v[i] = _tMin;
		}
	}
	return true;
}

//----------------------------------------------------------------------------------------------------------------------------------------
AABB::ClipResult AABB::Clip(const AABB& _other) const
{
	bool	bAllInside = true;
	bool	bAllOutside = true;

	for (int i = 0; i < 3; ++i)
	{
		if (vMin.v[i] >= _other.vMin.v[i] && vMax.v[i] <= _other.vMax.v[i])
		{
			bAllOutside = false;
		}
		else
		{
			bAllInside = false;
		}
	}
	
	if (bAllOutside)
	{
		return ClipResult::Outside;
	}
	else if (bAllInside)
	{
		return ClipResult::Inside;
	}
	return ClipResult::Intersect;
}
