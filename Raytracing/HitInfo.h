#pragma once

#include "Math.h"
#include "Materials/PDF.h"
#include "Objects/TraceableObject.h"

//----------------------------------------------------------------------------------------------------------------------------------------
class Material;

//----------------------------------------------------------------------------------------------------------------------------------------
struct HitInfo
{
	bool					isHit;
	double					distance;
	Vector3					incoming;
	Vector3					point;
	Vector3					surfaceNormal;
	Vector3					uvw;
	bool					frontFace;
	const Material*			material;
	PDFType					pdfType;
	const TraceableObject*	object;

	Vector3					attenuation;
	Vector3					scatterDirection;
	double					pdfValue;
	bool					isSpecular;
	const TraceableObjects*	pdfObjects;

	//------------------------------------------------------------------------------------------------------------------------------------
	HitInfo() :
		isHit(false),
		distance(DBL_MAX),
		isSpecular(false),
		pdfObjects(nullptr)
	{
	}

	inline void SetNormal(const Vector3& _rayDirection, const Vector3& _normal)
	{
		frontFace = Vector3::Dot(_rayDirection, _normal) < 0;
		surfaceNormal = frontFace ? _normal : -_normal;
	}
};

