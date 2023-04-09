#include "pch.h"
#include "Material.h"
#include "Objects/TraceableObject.h"

//----------------------------------------------------------------------------------------------------------------------------------------
Material::Material(const Color& _albedo) :
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
LambertMaterial::LambertMaterial(const Color& _albedo) :
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
MetalMaterial::MetalMaterial(const Color& _albedo, Vector3::Type _fuzziness) :
	Material(_albedo),
	fuzziness(_fuzziness)
{
}

//----------------------------------------------------------------------------------------------------------------------------------------
bool MetalMaterial::Scatter(const Ray& _ray, const HitInfo& _hitInfo, Vector3& _attenuation, Ray& _scattered) const
{
	_attenuation = albedo;

	Vector3 reflected;
	reflected = Vector3::Reflect(_ray.direction, _hitInfo.surfaceNormal) + fuzziness * Helper::RandomUnitSphere();

	_scattered = Ray(_hitInfo.point, reflected);

	return Vector3::Dot(reflected, _hitInfo.surfaceNormal) > 0;
}

//----------------------------------------------------------------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------------------------------------------------------------
DielectricMaterial::DielectricMaterial(Vector3::Type _refractionIndex) :
	Material(Color(1,1,1)),
	refractionIndex(_refractionIndex)
{
}

//----------------------------------------------------------------------------------------------------------------------------------------
bool DielectricMaterial::Scatter(const Ray& _ray, const HitInfo& _hitInfo, Vector3& _attenuation, Ray& _scattered) const
{
	_attenuation = albedo;

	Vector3::Type refractionRatio = _hitInfo.frontFace ? 1.0/refractionIndex : refractionIndex;

	Vector3::Type cos_theta = fmin(Vector3::Dot(-_ray.direction, _hitInfo.surfaceNormal), 1.0);
	Vector3::Type sin_theta = sqrt(1.0 - cos_theta * cos_theta);

	Vector3 direction;
	bool cantRefract = refractionRatio * sin_theta > 1.0;
	if (cantRefract || CalcReflectance(cos_theta, refractionRatio) > Helper::Random())
	{
		direction = Vector3::Reflect(_ray.direction, _hitInfo.surfaceNormal);
	}
	else 
	{
		direction = Vector3::Refract(_ray.direction, _hitInfo.surfaceNormal, refractionRatio);
	}
	
	_scattered = Ray(_hitInfo.point, direction);

	return true;
}

//----------------------------------------------------------------------------------------------------------------------------------------
Vector3::Type DielectricMaterial::CalcReflectance(Vector3::Type _cosine, Vector3::Type _refractionIndex)
{
	Vector3::Type r0 = (1.0 - _refractionIndex) / (1.0 + _refractionIndex);
	r0 = r0 * r0;
	return r0 + (1.0 - r0) * pow((1.0 - _cosine), 5.0);
}
