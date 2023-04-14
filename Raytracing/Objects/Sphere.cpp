#include "pch.h"
#include "Sphere.h"

//----------------------------------------------------------------------------------------------------------------------------------------
Sphere::Sphere(const Vector3& _center, double _radius, Material* _material) : TraceableObject(_center, _material)
{
	radius = _radius;
	Vector3 vRadius(radius,radius,radius);
	aabb = AABB(center-vRadius, center+vRadius);
}

//----------------------------------------------------------------------------------------------------------------------------------------
Sphere::~Sphere()
{
}

//----------------------------------------------------------------------------------------------------------------------------------------
//		x² + y² + z² = r²
// =>	|ray.pos + ray.dir * dst|² = r²
//----------------------------------------------------------------------------------------------------------------------------------------
void Sphere::Raycast(HitInfo& _hitInfo, const Ray& _ray, double _tMin, double _tMax) const
{
	double t0,t1;
	if (Intersect(_ray, center, radius, t0, t1))
	{
		double distance = t0;
		if (distance < _tMin || distance > _tMax)
		{
			distance = t1;
		}
		if (_tMin <= distance && distance <= _tMax)
		{
			_hitInfo.isHit = true;
			_hitInfo.distance = distance;

			_hitInfo.point = _ray.GetPoint(distance);

			Vector3 normal = (_hitInfo.point - center) / radius;
			_hitInfo.SetNormal(_ray.direction, normal);

			CalcSphereUV(normal, _hitInfo.uvw);

			OnHit(_hitInfo);
		}
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------
void Sphere::CalcSphereUV(const Vector3& _point, Vector3& _uvw)
{
	double theta = acos(-_point.y);
	double phi = atan2(-_point.z, _point.x) + M_PI;
	_uvw = Vector3(phi * 0.5 * M_1_PI, theta * M_1_PI, 0);
}

//----------------------------------------------------------------------------------------------------------------------------------------
bool Sphere::Intersect(const Ray& _ray, const Vector3& _center, double _radius, double& _t0, double& _t1)
{
	Vector3 offsetRayOrigin = _ray.origin - _center;

	// coefficients for quadratic equation
	double a = _ray.direction.LengthSq();
	double half_b = Vector3::Dot(offsetRayOrigin, _ray.direction);
	double c = offsetRayOrigin.LengthSq() - _radius * _radius;

	// quadratic discrinant
	double d = half_b * half_b - a * c;
	if (d < 0)
		return false;

	double sqrt_d = sqrt(d);
	_t0 = (-half_b - sqrt_d) / a;
	_t1 = (-half_b + sqrt_d) / a;;

	return true;
}