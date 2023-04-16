#include "pch.h"
#include "Material.h"
#include "../HitInfo.h"
#include "Texture.h"
#include "PDF.h"

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
bool LambertMaterial::Scatter(HitInfo& _hitInfo) const
{
	_hitInfo.attenuation = SampleAlbedo(_hitInfo);
	_hitInfo.pdfType = PDFType::Cosine;
	return true;
}

//----------------------------------------------------------------------------------------------------------------------------------------
double LambertMaterial::PDF(const HitInfo& _hitInfo) const
{
	const double cosTheta = Vector3::Dot(_hitInfo.surfaceNormal, _hitInfo.scatterDirection);
	return cosTheta < 0 ? 0 : cosTheta * M_1_PI;
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
bool MetalMaterial::Scatter(HitInfo& _hitInfo) const
{
	_hitInfo.isSpecular = true;

	Vector3 reflected = Vector3::Reflect(_hitInfo.incoming, _hitInfo.surfaceNormal);
	_hitInfo.scatterDirection = reflected + fuzziness * Helper::RandomUnitSphere();
	_hitInfo.scatterDirection.Normalize();

	_hitInfo.attenuation = SampleAlbedo(_hitInfo);

	return true;
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
bool DielectricMaterial::Scatter(HitInfo& _hitInfo) const
{
	_hitInfo.isSpecular = true;
	_hitInfo.attenuation = Color::ONE;

	double refractionRatio = _hitInfo.frontFace ? 1.0/refractionIndex : refractionIndex;

	double cosTheta = Vector3::Dot(-_hitInfo.incoming, _hitInfo.surfaceNormal);
	double sinTheta = sqrt(1.0 - cosTheta * cosTheta);

	bool cantRefract = refractionRatio * sinTheta > 1.0;
	double reflectance = CalcReflectance(cosTheta, refractionRatio);
	if (cantRefract || reflectance > Helper::Random())
	{
		_hitInfo.scatterDirection = Vector3::Reflect(_hitInfo.incoming, _hitInfo.surfaceNormal);
	}
	else 
	{
		_hitInfo.scatterDirection = Vector3::Refract(_hitInfo.incoming, _hitInfo.surfaceNormal, refractionRatio);
	}
	_hitInfo.scatterDirection.Normalize();
	
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
