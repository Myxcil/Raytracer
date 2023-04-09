#include "pch.h"
#include "Raytracer.h"

#include "Material.h"
#include "Objects/TraceableObject.h"
#include "Objects/Sphere.h"
#include "Objects/InfinitePlane.h"

//----------------------------------------------------------------------------------------------------------------------------------------
#define TO_RGB(r,g,b)          ((COLORREF)(((BYTE)(b)|((WORD)((BYTE)(g))<<8))|(((DWORD)(BYTE)(r))<<16)))

//----------------------------------------------------------------------------------------------------------------------------------------
Raytracer::Raytracer() :
	imageBuffer(nullptr),
	imageWidth(0),
	imageHeight(0),
	linesPerUpdate(8),
	rcpDimension(0,0,0),
	currLine(0),
	samplesPerPixel(30),
	maxRaycastDepth(8)
{
	InitScene();
}

//----------------------------------------------------------------------------------------------------------------------------------------
Raytracer::~Raytracer()
{
	for (size_t i = 0; i < traceableObjects.size(); ++i)
	{
		delete traceableObjects[i];
	}
	traceableObjects.clear();

	if (imageBuffer != nullptr)
	{
		delete[] imageBuffer;
		imageBuffer = nullptr;
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------
void Raytracer::InitScene()
{
	traceableObjects.push_back(new Sphere(Vector3(0.5, 1.0, 1), 1.0, new DielectricMaterial(1.5f)));
	traceableObjects.push_back(new Sphere(Vector3(0.5, 1.0, 1), -0.9, new DielectricMaterial(1.5f)));

	traceableObjects.push_back(new Sphere(Vector3(2.0, 0.5, -1.5f), 0.5, new LambertMaterial(Color(0.8, 0.1, 0.1f))));
	traceableObjects.push_back(new Sphere(Vector3(-4.5, 0.0, 0.5f), 3.0, new MetalMaterial(Color(0.2, 0.8, 0.3f), 0.1f)));

	traceableObjects.push_back(new InfinitePlane(Vector3(0, 0, 0), Vector3(0, 1, 0), new LambertMaterial(Color(0.1, 0.2, 0.5f))));
}

//----------------------------------------------------------------------------------------------------------------------------------------
void Raytracer::Resize(int _width, int _height)
{
	if (_width == imageWidth && _height == imageHeight)
		return;

	imageWidth = _width;
	imageHeight = _height;

	if (imageBuffer != nullptr)
	{
		delete[] imageBuffer;
		imageBuffer = nullptr;
	}

	unsigned long size = imageWidth * imageHeight * 4;
	imageBuffer = new unsigned char[size];
	ZeroMemory(imageBuffer, size);

	float aspect = static_cast<float>(_width) / _height;
	camera.SetAspect(aspect);

	currLine = 0;

	rcpDimension = Vector3(1.0f/(imageWidth-1), 1.0f/(imageHeight-1), 0);
}

//----------------------------------------------------------------------------------------------------------------------------------------
static const bool showObjectNormals = false;

//----------------------------------------------------------------------------------------------------------------------------------------
void Raytracer::Update(float _deltaTime)
{
	for(int i=0; i < linesPerUpdate && currLine < imageHeight; ++i)
	{
		TraceScene(currLine);
		++currLine;
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------
void Raytracer::TraceScene(int _line)
{
	Ray ray;

	for (int x = 0; x < imageWidth; ++x)
	{
		Color finalColor;
		if (!showObjectNormals)
		{
			for (int i = 0; i < samplesPerPixel; ++i)
			{
				Vector3::Type tx = rcpDimension.x * (Helper::Random() + x);
				Vector3::Type ty = 1.0f - rcpDimension.y * (Helper::Random() + _line);

				camera.CalculateRay(tx, ty, ray);

				Color color;
				EvaluateColor(color, ray, camera.GetNearPlane(), FLT_MAX, maxRaycastDepth);
				finalColor += color;
			}

			SetPixel(x, _line, finalColor);
		}
		else
		{
			Vector3::Type tx = rcpDimension.x * x;
			Vector3::Type ty = 1.0f - rcpDimension.y * _line;

			camera.CalculateRay(tx, ty, ray);

			HitInfo hitInfo;
			RaycastObjects(hitInfo, ray, camera.GetNearPlane(), FLT_MAX);
			if (hitInfo.isHit)
			{
				finalColor = hitInfo.surfaceNormal;
			}
			else
			{
				finalColor = ray.direction;
			}
			finalColor *= 0.5f;
			finalColor += Color(0.5, 0.5, 0.5f);

			SetPixelDirect(x, _line, finalColor);
		}
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------
void Raytracer::EvaluateColor(Color& _color, const Ray& _ray, Vector3::Type _tMin, Vector3::Type _tMax, int depth)
{
	if (depth > 0)
	{
		HitInfo hitInfo;
		RaycastObjects(hitInfo, _ray, _tMin, _tMax);
		if (hitInfo.isHit)
		{
			Ray scattered;
			Color attenuation;
			if (hitInfo.material->Scatter(_ray, hitInfo, attenuation, scattered))
			{
				Color nextColor;
				EvaluateColor(nextColor, scattered, 0.001, DBL_MAX, depth - 1);
				_color = attenuation * nextColor;
			}
			else
			{
				_color = Color(0,0,0);
			}
		}
		else
		{
			SampleEnviroment(_color, _ray.direction);
		}
	}
	else
	{
		_color = Color(0,0,0);
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------
void Raytracer::RaycastObjects(HitInfo& _hitInfo, const Ray& _ray, Vector3::Type _tMin, Vector3::Type _tMax)
{
	_hitInfo.isHit = false;
	_hitInfo.distance = DBL_MAX;

	HitInfo objHit;
	for (size_t i=0; i < traceableObjects.size(); ++i)
	{
		traceableObjects[i]->Raycast(objHit, _ray, _tMin, _tMax);
		if (objHit.isHit && objHit.distance < _hitInfo.distance)
		{
			_hitInfo = objHit;
		}
	}
}

/*
//----------------------------------------------------------------------------------------------------------------------------------------
const Vector3 colorZenith = { 0.6, 0.6, 1.0 };
const Vector3 colorHorizon = { 0.8, 0.8, 1.0 };
const Vector3 colorGround = { 0.5, 0.5, 0.5 };

const Vector3 sunDirection = { -0.38, -0.64, 0.38 };
const float sunFocus = 4.0;
const float sunIntensity = 1.0;

//----------------------------------------------------------------------------------------------------------------------------------------
void Raytracer::SampleEnviroment(float* _color, const float* _rayDirection)
{
	float skyGradientT = powf(FastMath::SmwoothStep(0.0, 0.4, _rayDirection[1]), 0.35);

	Vector3 skyGradient;
	Vec3_Lerp(skyGradient, colorHorizon, colorZenith, skyGradientT);
	
	float sun = powf(FastMath::Max(0.0, -Vec3_Dot(_rayDirection, sunDirection)), sunFocus) * sunIntensity;

	float groundToSkyT = FastMath::SmwoothStep(0.0, 0.01, _rayDirection[1]);
	float sunMask = groundToSkyT >= 1;
	Vec3_Lerp(_color, colorGround, skyGradient, groundToSkyT);

	Vector3 sunColor;
	Vec3_Set(sunColor,1,1,1);
	Vec3_Mul(sunColor, sun * sunMask);
	Vec3_Add(_color, sunColor);
}
*/

//----------------------------------------------------------------------------------------------------------------------------------------
const Color colorA = Vector3(1.0, 1.0, 1.0);
const Color colorB = Vector3(0.5, 0.7, 1.0);

//----------------------------------------------------------------------------------------------------------------------------------------
void Raytracer::SampleEnviroment(Color& _color, const Vector3& _rayDirection)
{
	Vector3::Type t = 0.5 + 0.5 * _rayDirection.y;
	_color = Vector3::Lerp(colorA, colorB, t);
}

//----------------------------------------------------------------------------------------------------------------------------------------
void Raytracer::SetPixel(int _x, int _y, const Color& _color)
{
	float scale = 1.0f / samplesPerPixel;

	unsigned char bR = static_cast<unsigned char>(sqrt(_color.x * scale) * 255.0f);
	unsigned char bG = static_cast<unsigned char>(sqrt(_color.y * scale) * 255.0f);
	unsigned char bB = static_cast<unsigned char>(sqrt(_color.z * scale) * 255.0f);

	UINT32* pixel = static_cast<UINT32*>(imageBuffer);
	pixel[_x + _y * imageWidth] = TO_RGB(bR, bG, bB);
}

//----------------------------------------------------------------------------------------------------------------------------------------
void Raytracer::SetPixelDirect(int _x, int _y, const Color& _color)
{
	unsigned char bR = static_cast<unsigned char>(_color.x * 255.0f);
	unsigned char bG = static_cast<unsigned char>(_color.y * 255.0f);
	unsigned char bB = static_cast<unsigned char>(_color.z * 255.0f);

	UINT32* pixel = static_cast<UINT32*>(imageBuffer);
	pixel[_x + _y * imageWidth] = TO_RGB(bR, bG, bB);
}
