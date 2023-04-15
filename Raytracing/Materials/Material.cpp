#include "pch.h"
#include "Material.h"
#include "../Objects/TraceableObject.h"
#include "Texture.h"

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
	_scatterInfo.direction = Helper::RandomCosineHemisphere(_hitInfo.surfaceNormal);
	
	const double cosTheta = fmax(0,Vector3::Dot(_hitInfo.surfaceNormal, _scatterInfo.direction));
	_scatterInfo.attenuation = SampleAlbedo(_hitInfo) * cosTheta * M_1_PI;

	return true;
}

//----------------------------------------------------------------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------------------------------------------------------------
MetalMaterial::MetalMaterial(const Texture* _albedo, double _fuzziness) :
	Material(_albedo),
	fuzziness(_fuzziness)
{
}

//----------------------------------------------------------------------------------------------------------------------------------------
bool MetalMaterial::Scatter(const Ray& _ray, const HitInfo& _hitInfo, ScatterInfo& _scatterInfo) const
{
	_scatterInfo.attenuation = SampleAlbedo(_hitInfo);
	_scatterInfo.direction = Vector3::Reflect(_ray.direction, _hitInfo.surfaceNormal) + fuzziness * Helper::RandomUnitSphere();
	return Vector3::Dot(_scatterInfo.direction, _hitInfo.surfaceNormal) > 0;
}

//----------------------------------------------------------------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------------------------------------------------------------
DielectricMaterial::DielectricMaterial(double _refractionIndex) :
	Material(nullptr),
	refractionIndex(_refractionIndex)
{
}

//----------------------------------------------------------------------------------------------------------------------------------------
bool DielectricMaterial::Scatter(const Ray& _ray, const HitInfo& _hitInfo, ScatterInfo& _scatterInfo) const
{
	_scatterInfo.attenuation = Color(1,1,1);

	double refractionRatio = _hitInfo.frontFace ? 1.0/refractionIndex : refractionIndex;

	double cos_theta = fmin(Vector3::Dot(-_ray.direction, _hitInfo.surfaceNormal), 1.0);
	double sin_theta = sqrt(1.0 - cos_theta * cos_theta);

	bool cantRefract = refractionRatio * sin_theta > 1.0;
	double reflectance = CalcReflectance(cos_theta, refractionRatio);
	if (cantRefract || reflectance > Helper::Random())
	{
		_scatterInfo.direction = Vector3::Reflect(_ray.direction, _hitInfo.surfaceNormal);
	}
	else 
	{
		_scatterInfo.direction = Vector3::Refract(_ray.direction, _hitInfo.surfaceNormal, refractionRatio);
	}
	
	return true;
}

//----------------------------------------------------------------------------------------------------------------------------------------
double DielectricMaterial::CalcReflectance(double _cosine, double _refractionIndex)
{
	double r0 = (1.0 - _refractionIndex) / (1.0 + _refractionIndex);
	r0 = r0 * r0;
	return r0 + (1.0 - r0) * pow((1.0 - _cosine), 5.0);
}

//----------------------------------------------------------------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------------------------------------------------------------
DiffuseLight::DiffuseLight(const Color& _emit, bool _visible) :
	Material(nullptr),
	emit(_emit),
	visible(_visible)
{
}
