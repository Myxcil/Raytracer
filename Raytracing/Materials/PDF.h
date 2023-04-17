#pragma once

//----------------------------------------------------------------------------------------------------------------------------------------
struct HitInfo;

//------------------------------------------------------------------------------------------------------------------------------------
enum class PDFType
{
	None,
	Cosine,
	Objects,
};

//----------------------------------------------------------------------------------------------------------------------------------------
class PDF
{
public:
	//------------------------------------------------------------------------------------------------------------------------------------
	static double		Value(PDFType _type, const HitInfo& _hitInfo);
	static Vector3		Generate(PDFType _type, const HitInfo& _hitInfo);
};