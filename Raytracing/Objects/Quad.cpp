#include "pch.h"
#include "Quad.h"
#include "../HitInfo.h"

//----------------------------------------------------------------------------------------------------------------------------------------
Quad::Quad(const Vector3& _center, const Vector3& _normal, const Vector3& _size, Material* _material) :
	InfinitePlane(_center, _normal, true, _material),
	size(_size)
{
	if (fabs(normal.y) > fabs(normal.z))
	{
		u = Vector3(normal.y, normal.z, normal.x);
	}
	else
	{
		u = Vector3(normal.z, normal.y, normal.x);
	}
	v = Vector3::Cross(u, normal);

	area = size.x * size.y;

	//ConstructAABB();
}

//----------------------------------------------------------------------------------------------------------------------------------------
void Quad::Raycast(HitInfo& _hitInfo, const Ray& _ray, double _tMin, double _tMax) const
{
	InfinitePlane::Raycast(_hitInfo, _ray, _tMin, _tMax);
	if (_hitInfo.isHit)
	{
		Vector3 local = _hitInfo.point - center;
		double tu = 0.5 + 0.5 * (Vector3::Dot(local,u) / size.x);
		double tv = 0.5 + 0.5 * (Vector3::Dot(local,v) / size.y);
		_hitInfo.isHit = (tu >= 0 && tu <= 1) && (tv >= 0 && tv <= 1);
		_hitInfo.uvw = Vector3(tu,tv,0);
	}
}
	
//----------------------------------------------------------------------------------------------------------------------------------------
void Quad::ConstructAABB()
{
	Vector3 halfSize = size;
	halfSize.z = 0.001;

	Vector3 points[8];
	points[0] = center - u * halfSize.x - v * halfSize.y - normal * halfSize.z;
	points[1] = center + u * halfSize.x - v * halfSize.y - normal * halfSize.z;
	points[2] = center - u * halfSize.x + v * halfSize.y - normal * halfSize.z;
	points[3] = center + u * halfSize.x + v * halfSize.y - normal * halfSize.z;
	points[4] = center - u * halfSize.x - v * halfSize.y + normal * halfSize.z;
	points[5] = center + u * halfSize.x - v * halfSize.y + normal * halfSize.z;
	points[6] = center - u * halfSize.x + v * halfSize.y + normal * halfSize.z;
	points[7] = center + u * halfSize.x + v * halfSize.y + normal * halfSize.z;

	aabb = AABB(points[0], points[0]);
	for (int i = 1; i < 8; ++i)
	{
		aabb.Merge(points[i]);
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------
double Quad::PDFValue(const HitInfo& _hitInfo) const
{
	Ray ray = Ray(_hitInfo.point, _hitInfo.scatterDirection);
	
	HitInfo temp;
	Raycast(temp, ray, 0.0001, DBL_MAX);
	if (!temp.isHit)
		return 0;

	const double distSq = temp.distance * temp.distance;
	const double cosine = abs(Vector3::Dot(temp.surfaceNormal, _hitInfo.scatterDirection));

	return distSq / (cosine * area);
}

//----------------------------------------------------------------------------------------------------------------------------------------
Vector3 Quad::PDFGenerate(const HitInfo& _hitInfo) const
{
	const double s = Helper::Random(-1,1);
	const double t = Helper::Random(-1,1);
	Vector3 p = center + s * u * size.x + t * v * size.y;
	p -= _hitInfo.point;
	p.Normalize();
	return p;
}
