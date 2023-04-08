#include "pch.h"
#include "Material.h"
#include "Objects/TraceableObject.h"

//----------------------------------------------------------------------------------------------------------------------------------------
Material::Material(const Vector3& _albedo) :
	numRefs(0),
	albedo(_albedo)
{
}

//----------------------------------------------------------------------------------------------------------------------------------------
Material::~Material()
{
}

//----------------------------------------------------------------------------------------------------------------------------------------
void Material::Release()
{
	if (--numRefs == 0)
	{
		delete this;
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------------------------------------------------------------
LambertMaterial::LambertMaterial(const Vector3& _albedo) :
	Material(_albedo)
{
}

//----------------------------------------------------------------------------------------------------------------------------------------
bool LambertMaterial::Scatter(const Ray& _ray, const HitInfo& _hitInfo, Vector3& _attenuation, Ray& _scattered) const
{
	_attenuation = albedo;

	Vector3 scatterDir = _hitInfo.surfaceNormal + Helper::RandomUnitHemisphere(_hitInfo.surfaceNormal);
	if (scatterDir.NearZero())
	{
		scatterDir = _hitInfo.surfaceNormal;
	}
	_scattered = Ray(_hitInfo.point, scatterDir);

	return true;
}

//----------------------------------------------------------------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------------------------------------------------------------
MetalMaterial::MetalMaterial(const Vector3& _albedo) :
	Material(_albedo)
{
}

//----------------------------------------------------------------------------------------------------------------------------------------
bool MetalMaterial::Scatter(const Ray& _ray, const HitInfo& _hitInfo, Vector3& _attenuation, Ray& _scattered) const
{
	_attenuation = albedo;

	Vector3 reflected;
	reflected = Vector3::Reflect(_ray.direction, _hitInfo.surfaceNormal);
	reflected.Normalize();

	_scattered = Ray(_hitInfo.point, reflected);

	return reflected.Dot(_hitInfo.surfaceNormal) > 0;
}