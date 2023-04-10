#include "pch.h"
#include "AABB.h"
#include "TraceableObject.h"

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
		}
	}
	return true;
}

//----------------------------------------------------------------------------------------------------------------------------------------
void AABB::Raycast(HitInfo& _hitInfo, const Ray& _ray, Vector3::Type _tMin, Vector3::Type _tMax) const
{
	if (!Hit(_ray, _tMin, _tMax))
		return;

	Vector3 rMin = (vMin - _ray.origin) / _ray.direction;
	Vector3 rMax = (vMax - _ray.origin) / _ray.direction;
	
	Vector3 t0 = Vector3::Min(rMin,rMax);
	Vector3 t1 = Vector3::Max(rMin,rMax);

	Vector3::Type tNear = max(max(t0.x,t0.y),t0.z);
	Vector3::Type tFar = min(min(t1.x,t1.y),t1.z);

	if (tNear < tFar)
	{
		Vector3::Type distance = tNear >= 0 ? tNear : tFar;

		_hitInfo.isHit = true;
		_hitInfo.distance = distance;
		_hitInfo.point = _ray.GetPoint(distance);

		Vector3 boxCenter = (vMin + vMax) / 2.0;
		Vector3 boxVec = _hitInfo.point - boxCenter;
		Vector3::Type maxAxis = fmax(fmax(fabs(boxVec.x),fabs(boxVec.y)),fabs(boxVec.z));
		boxVec /= maxAxis;

		Vector3 normal = boxVec * 1.000001;
		normal.Floor();
		normal.Normalize();

		_hitInfo.SetNormal(_ray.direction, normal);
		_hitInfo.uvw = (_hitInfo.point - boxCenter) / Size();
	}
}
