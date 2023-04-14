#pragma once

//----------------------------------------------------------------------------------------------------------------------------------------
struct HitInfo;
class Texture;

//----------------------------------------------------------------------------------------------------------------------------------------
struct ScatterInfo
{
	Vector3			attenuation;
	Vector3			direction;
};

//----------------------------------------------------------------------------------------------------------------------------------------
class Material
{
public:
	//------------------------------------------------------------------------------------------------------------------------------------
	virtual ~Material();

	//------------------------------------------------------------------------------------------------------------------------------------
	void			AddRef()			{ ++numRefs; }
	void			Release();

	//------------------------------------------------------------------------------------------------------------------------------------
	virtual bool	Scatter(const Ray& _ray, const HitInfo& _hitInfo, ScatterInfo& _scatterInfo) const = 0;
	virtual Color	Emitted(const Vector3& _uvw, const Vector3& _point) const { return Color(0,0,0); }

protected:
	//------------------------------------------------------------------------------------------------------------------------------------
	Material(const Texture* _albedo);
	Color			SampleAlbedo(const HitInfo& _hitInfo) const;

private:
	//------------------------------------------------------------------------------------------------------------------------------------
	int				numRefs;
	const Texture*	albedo;
};

//----------------------------------------------------------------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------------------------------------------------------------
class LambertMaterial : public Material
{
public:
	//------------------------------------------------------------------------------------------------------------------------------------
	LambertMaterial(const Texture* _albedo);
	bool			Scatter(const Ray& _ray, const HitInfo& _hitInfo, ScatterInfo& _scatterInfo) const override;
};

//----------------------------------------------------------------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------------------------------------------------------------
class MetalMaterial : public Material
{
public:
	//------------------------------------------------------------------------------------------------------------------------------------
	MetalMaterial(const Texture* _albedo, double _fuzziness);
	bool			Scatter(const Ray& _ray, const HitInfo& _hitInfo, ScatterInfo& _scatterInfo) const override;

private:
	//------------------------------------------------------------------------------------------------------------------------------------
	double	fuzziness;
};

//----------------------------------------------------------------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------------------------------------------------------------
class DielectricMaterial : public Material
{
public:
	//------------------------------------------------------------------------------------------------------------------------------------
	DielectricMaterial(double _refractionIndex);
	bool			Scatter(const Ray& _ray, const HitInfo& _hitInfo, ScatterInfo& _scatterInfo) const override;

private:
	//------------------------------------------------------------------------------------------------------------------------------------
	static double	CalcReflectance(double _cosine, double _refractionIndex);

private:
	//------------------------------------------------------------------------------------------------------------------------------------
	double	refractionIndex;
};

//----------------------------------------------------------------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------------------------------------------------------------
class DiffuseLight : public Material
{
public:
	//------------------------------------------------------------------------------------------------------------------------------------
	DiffuseLight(const Color& _emit, bool _visible = true);

	//------------------------------------------------------------------------------------------------------------------------------------
	bool	Scatter(const Ray& _ray, const HitInfo& _hitInfo, ScatterInfo& _scatterInfo) const override { return false; }
	Color	Emitted(const Vector3& _uvw, const Vector3& _point) const override							{ return emit; }

private:
	//------------------------------------------------------------------------------------------------------------------------------------
	Color	emit;
	bool	visible;
};