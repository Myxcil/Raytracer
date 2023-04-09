#include "pch.h"
#include "Texture.h"

//----------------------------------------------------------------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------------------------------------------------------------
Texture::Texture()
{
}

//----------------------------------------------------------------------------------------------------------------------------------------
Texture::~Texture()
{
}

//----------------------------------------------------------------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------------------------------------------------------------
ConstantColor::ConstantColor(const Color& _color) :
	color(_color)
{
}

//----------------------------------------------------------------------------------------------------------------------------------------
const ConstantColor ConstantColor::RED		= ConstantColor(Color(1,0,0));
const ConstantColor ConstantColor::GREEN	= ConstantColor(Color(0,1,0));
const ConstantColor ConstantColor::BLUE		= ConstantColor(Color(0,0,1));
const ConstantColor ConstantColor::YELLOW	= ConstantColor(Color(1,1,0));
const ConstantColor ConstantColor::BLACK	= ConstantColor(Color(0,0,0));
const ConstantColor ConstantColor::WHITE	= ConstantColor(Color(1,1,1));
const ConstantColor ConstantColor::GREY		= ConstantColor(Color(0.5,0.5,0.5));

//----------------------------------------------------------------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------------------------------------------------------------
CheckerTexture::CheckerTexture(const Texture* _colorA, const Texture* _colorB, const Vector3& _scale) :
	colorA(_colorA),
	colorB(_colorB),
	scale(_scale)
{
}

//----------------------------------------------------------------------------------------------------------------------------------------
Color CheckerTexture::Evaluate(const Vector3& _uvw, const Vector3& _point) const
{
	Vector3 phase = (_uvw * 2.0f * M_PI) * scale;
	int ix = (int)floor(phase.x);
	int iy = (int)floor(phase.y);

	const Texture* source;
	if ((ix + iy) % 2 == 0)
	{
		source = colorA;
	}
	else
	{
		source = colorB;
	}
	
	return source->Evaluate(_uvw, _point);
}