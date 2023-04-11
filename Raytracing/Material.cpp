#include "pch.h"
#include "Material.h"
#include "Objects/TraceableObject.h"
#include "Textures/Texture.h"

//----------------------------------------------------------------------------------------------------------------------------------------
Material::Material(const Texture* _albedo) :
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
Color Material::SampleAlbedo(const HitInfo& _hitInfo) const
{
	return albedo != nullptr ? albedo->Evaluate(_hitInfo.uvw, _hitInfo.point) : Color(0,0,0);
}

//----------------------------------------------------------------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------------------------------------------------------------
LambertMaterial::LambertMaterial(const Texture* _albedo) :
	Material(_albedo)
{
}

//----------------------------------------------------------------------------------------------------------------------------------------
bool LambertMaterial::Scatter(const Ray& _ray, const HitInfo& _hitInfo, ScatterInfo& _scatterInfo) const
{
	_scatterInfo.isSpecular = false;
	_scatterInfo.attenuation = SampleAlbedo(_hitInfo);
	_scatterInfo.cosinePDF.Set(_hitInfo.surfaceNormal);
	return true;
}

//----------------------------------------------------------------------------------------------------------------------------------------
Vector3::Type LambertMaterial::ScatteringPDF(const Ray& _ray, const HitInfo& _hitInfo, const Ray& _scattered) const
{
	Vector3::Type cosine = Vector3::Dot(_hitInfo.surfaceNormal, _scattered.direction);
	return cosine < 0 ? 0 : cosine * M_1_PI;
}

/*
//----------------------------------------------------------------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------------------------------------------------------------
MetalMaterial::MetalMaterial(const Texture* _albedo, Vector3::Type _fuzziness) :
	Material(_albedo),
	fuzziness(_fuzziness)
{
}

//----------------------------------------------------------------------------------------------------------------------------------------
bool MetalMaterial::Scatter(const Ray& _ray, const HitInfo& _hitInfo, Vector3& _attenuation, Ray& _scattered) const
{
	_attenuation = SampleAlbedo(_hitInfo);

	Vector3 reflected;
	reflected = Vector3::Reflect(_ray.direction, _hitInfo.surfaceNormal) + fuzziness * Helper::RandomUnitSphere();

	_scattered = Ray(_hitInfo.point, reflected);

	return Vector3::Dot(reflected, _hitInfo.surfaceNormal) > 0;
}

//----------------------------------------------------------------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------------------------------------------------------------
DielectricMaterial::DielectricMaterial(Vector3::Type _refractionIndex) :
	Material(nullptr),
	refractionIndex(_refractionIndex)
{
}

//----------------------------------------------------------------------------------------------------------------------------------------
bool DielectricMaterial::Scatter(const Ray& _ray, const HitInfo& _hitInfo, Vector3& _attenuation, Ray& _scattered) const
{
	_attenuation = Color(1,1,1);

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
*/

//----------------------------------------------------------------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------------------------------------------------------------
DiffuseLight::DiffuseLight(const Color& _emit, bool _visible) :
	Material(nullptr),
	emit(_emit),
	visible(_visible)
{
}
