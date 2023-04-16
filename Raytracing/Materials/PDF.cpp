#include "pch.h"
#include "PDF.h"
#include "../HitInfo.h"
#include "../Objects/TraceableObject.h"

//----------------------------------------------------------------------------------------------------------------------------------------
double PDF::Value(PDFType _type, const HitInfo& _hitInfo)
{
	switch (_type)
	{
	case PDFType::Cosine:
	{
		double cosine = Vector3::Dot(_hitInfo.surfaceNormal, _hitInfo.scatterDirection);
		return cosine <= 0 ? 0 : cosine / M_PI;
	}
	
	case PDFType::Objects:
	{
		const TraceableObjects& list = *_hitInfo.pdfObjects;
		double sum = 0;
		for (size_t i = 0; i < list.size(); ++i)
		{
			sum += list[i]->PDFValue(_hitInfo);
		}
		return sum / list.size();
	}

	default:

		return 0;
	}
}

//------------------------------------------------------------------------------------------------------------------------------------
Vector3 PDF::Generate(PDFType _type, const HitInfo& _hitInfo)
{
	switch (_type)
	{
	case PDFType::Cosine:
	{
		return Transfrom(_hitInfo.surfaceNormal).Rotate(Helper::RandomCosineHemisphere());
	}

	case PDFType::Objects:
	{
		const TraceableObjects& list = *_hitInfo.pdfObjects;
		const size_t index = static_cast<size_t>(Helper::Random() * list.size());
		return list[index]->PDFGenerate(_hitInfo);
	}
	
	default:
		return Vector3::ZERO;		
	}
}
