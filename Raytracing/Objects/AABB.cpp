#include "pch.h"
#include "AABB.h"
#include "TraceableObject.h"
#include "../HitInfo.h"

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
bool AABB::Hit(const Ray& _ray, double _tMin, double _tMax) const
{
	if (isValid)
	{
		for (int i = 0; i < 3; ++i)
		{
			if (_ray.direction.v[i] != 0)
			{
				double invD = 1.0 / _ray.direction.v[i];
				double t0 = (vMin.v[i] - _ray.origin.v[i]) * invD;
				double t1 = (vMax.v[i] - _ray.origin.v[i]) * invD;
				if (invD < 0)
				{
					std::swap(t0, t1);
				}
				_tMin = t0 > _tMin ? t0 : _tMin;
				_tMax = t1 < _tMin ? t1 : _tMax;
				if (_tMax <= _tMin)
					return false;
			}
		}
	}
	return true;
}

//----------------------------------------------------------------------------------------------------------------------------------------
void AABB::Raycast(HitInfo& _hitInfo, const Ray& _ray, double _tMin, double _tMax) const
{
	if (!Hit(_ray, _tMin, _tMax))
		return;

	Vector3 rMin = Vector3::Div(vMin - _ray.origin, _ray.direction);
	Vector3 rMax = Vector3::Div(vMax - _ray.origin, _ray.direction);

	Vector3 t0 = Vector3::Min(rMin, rMax);
	Vector3 t1 = Vector3::Max(rMin, rMax);

	double tNear = t0.Max();
	double tFar =  t1.Min();

	if (tNear < tFar)
	{
		double distance = tNear >= 0 ? tNear : tFar;;

		_hitInfo.isHit = true;
		_hitInfo.distance = distance;
		_hitInfo.point = _ray.GetPoint(distance);

		Vector3 boxCenter = (vMin + vMax) / 2.0;
		Vector3 boxVec = _hitInfo.point - boxCenter;
		
		Vector3 absBox = boxVec;
		absBox.Abs();
		if (absBox.x >= absBox.y && absBox.x >= absBox.z)
		{
			boxVec.y = boxVec.z = 0;
		}
		else if (absBox.y >= absBox.x && absBox.y >= absBox.z)
		{
			boxVec.x = boxVec.z = 0;
		}
		else
		{
			boxVec.x = boxVec.y = 0;
		}
		boxVec.Normalize();
		_hitInfo.SetNormal(_ray.direction, boxVec);
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------
void AABB::EnsureMinSize(double _minSize)
{
	Vector3 size = Size();
	double halfMin = 0.5 + _minSize;
	if (size.x < _minSize)
	{
		vMin.x -= halfMin;
		vMax.x += halfMin;
	}
	if (size.y < _minSize)
	{
		vMin.y -= halfMin;
		vMax.y += halfMin;
	}
	if (size.z < _minSize)
	{
		vMin.z -= halfMin;
		vMax.z += halfMin;
	}
}
