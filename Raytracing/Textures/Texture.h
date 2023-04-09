#pragma once

//----------------------------------------------------------------------------------------------------------------------------------------
class Texture
{
public:
	//------------------------------------------------------------------------------------------------------------------------------------
	virtual ~Texture();
	virtual Color	Evaluate(const Vector3& _uvw, const Vector3& _point) const = 0;

protected:
	//------------------------------------------------------------------------------------------------------------------------------------
	Texture();
};

//----------------------------------------------------------------------------------------------------------------------------------------
class ConstantColor : public Texture
{
public: 
	//------------------------------------------------------------------------------------------------------------------------------------
	ConstantColor(const Color& _color);
	Color	Evaluate(const Vector3& _uvw, const Vector3& _point) const override { return color; }

	//------------------------------------------------------------------------------------------------------------------------------------
	static const ConstantColor RED;
	static const ConstantColor GREEN;
	static const ConstantColor BLUE;
	static const ConstantColor YELLOW;
	static const ConstantColor BLACK;
	static const ConstantColor WHITE;
	static const ConstantColor GREY;

private:
	//------------------------------------------------------------------------------------------------------------------------------------
	Color		color;
};

//----------------------------------------------------------------------------------------------------------------------------------------
class CheckerTexture : public Texture
{
public:
	//------------------------------------------------------------------------------------------------------------------------------------
	CheckerTexture(const Texture* _colorA, const Texture* _colorB, const Vector3& _scale = Vector3(1,1,1));
	
	Color	Evaluate(const Vector3& _uvw, const Vector3& _point) const override;

private:
	//------------------------------------------------------------------------------------------------------------------------------------
	const Texture*	colorA;
	const Texture*	colorB;
	Vector3			scale;
};