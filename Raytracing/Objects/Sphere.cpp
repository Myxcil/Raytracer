#include "pch.h"
#include "Sphere.h"

//----------------------------------------------------------------------------------------------------------------------------------------
Sphere::Sphere(const Vector3& _center, Vector3::Type _radius, Material* _material) : TraceableObject(_center, _material)
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
void Sphere::Raycast(HitInfo& _hitInfo, const Ray& _ray, Vector3::Type _tMin, Vector3::Type _tMax) const
{
	Vector3::Type t0,t1;
	if (Intersect(_ray, center, radius, t0, t1))
	{
		Vector3::Type distance = t0;
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
	Vector3::Type theta = acos(-_point.y);
	Vector3::Type phi = atan2(-_point.z, _point.x) + M_PI;
	_uvw = Vector3(phi * 0.5 * M_1_PI, theta * M_1_PI, 0);
}

//----------------------------------------------------------------------------------------------------------------------------------------
bool Sphere::Intersect(const Ray& _ray, const Vector3& _center, Vector3::Type _radius, Vector3::Type& _t0, Vector3::Type& _t1)
{
	Vector3 offsetRayOrigin = _ray.origin - _center;

	// coefficients for quadratic equation
	Vector3::Type a = _ray.direction.LengthSq();
	Vector3::Type half_b = Vector3::Dot(offsetRayOrigin, _ray.direction);
	Vector3::Type c = offsetRayOrigin.LengthSq() - _radius * _radius;

	// quadratic discrinant
	Vector3::Type d = half_b * half_b - a * c;
	if (d < 0)
		return false;

	Vector3::Type sqrt_d = sqrt(d);
	_t0 = (-half_b - sqrt_d) / a;
	_t1 = (-half_b + sqrt_d) / a;;

	return true;
}

//----------------------------------------------------------------------------------------------------------------------------------------
Vector3::Type Sphere::CalcPDFValue(const Vector3& _origin, const Vector3& _direction) const
{
	Ray ray(_origin, _direction);
	HitInfo hitInfo;
	Raycast(hitInfo, ray, 0.001, DBL_MAX);
	if (!hitInfo.isHit)
		return 0;

	Vector3 diff = center - _origin;
	Vector3::Type cos_theta_max = sqrt(1.0 - radius*radius/diff.LengthSq());
	Vector3::Type solidAngle = 2.0 * M_PI * (1.0 - cos_theta_max);
	
	return 1.0 / solidAngle;
}

//----------------------------------------------------------------------------------------------------------------------------------------
Vector3 Sphere::GetRandomDirection(const Vector3& _origin) const
{
	Vector3 direction = center - _origin;
	Vector3::Type distSq = direction.LengthSq();

	return ONB(direction).Transform(Helper::RandomToSphere(radius,distSq));
}