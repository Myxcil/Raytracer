#pragma once

//----------------------------------------------------------------------------------------------------------------------------------------
class FastMath
{
public:
	//----------------------------------------------------------------------------------------------------------------------------------------
	static const float		EPSILON;

	//----------------------------------------------------------------------------------------------------------------------------------------
	static int				FloatToInt(const float _f);
	static float			IntToFloat(const int _i);

	//----------------------------------------------------------------------------------------------------------------------------------------
	static float			Sign(const float _f);
	static int				Compare(const float _f, const float _g);
	static float			Saturate(const float _f);
	static float			Abs(const float _f);
	static float			Sqrt(const float _f);

	//----------------------------------------------------------------------------------------------------------------------------------------
	static float			Log(const float _f);

	//----------------------------------------------------------------------------------------------------------------------------------------
	static float			Sin(const float _radians);
	static float			Cos(const float _radians);
	static float			Tan(const float _radians);

	//----------------------------------------------------------------------------------------------------------------------------------------
	static float			Deg2Rad(const float _degree);

	//----------------------------------------------------------------------------------------------------------------------------------------
	static float			RandomValue(unsigned int& _state);
	static float			RandomValueNormalDistribution(unsigned int& _state);

	//----------------------------------------------------------------------------------------------------------------------------------------
	static float			SmwoothStep(float a, float b, float t);
	static float			Min(float a, float b);
	static float			Max(float a, float b);

	//----------------------------------------------------------------------------------------------------------------------------------------
	static DirectX::SimpleMath:: Vector3	RandomDirection(unsigned int& _state);
	static DirectX::SimpleMath:: Vector3	RandomDirectionInHemisphere(const DirectX::SimpleMath:: Vector3& _normal, unsigned int& _state);
};
