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
	samplesPerPixel(100),
	maxRaycastDepth(16)
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
	traceableObjects.push_back(new Sphere(Vector3(0, 1.0f, 0), 1.0f, new LambertMaterial(Vector3(0.7f, 0.7f, 0.0f))));
	traceableObjects.push_back(new Sphere(Vector3(1.0f, 0.5f, -1.5f), 0.25f, new LambertMaterial(Vector3(0.8f, 0.1f, 0.1f))));
	traceableObjects.push_back(new Sphere(Vector3(-5.5f, 0.0f, 0.5f), 3.0f, new MetalMaterial(Vector3(0.2f, 0.8f, 0.3f))));

	traceableObjects.push_back(new InfinitePlane(Vector3(0, 0, 0), Vector3(0, 1, 0), new LambertMaterial(Vector3(0.1f, 0.2f, 0.5f))));
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
		TraceScene();
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------
void Raytracer::TraceScene()
{
	Ray ray;

	for (int x = 0; x < imageWidth; ++x)
	{
		Vector3 finalColor;
		if (!showObjectNormals)
		{
			for (int i = 0; i < samplesPerPixel; ++i)
			{
				Vector3::Type tx = rcpDimension.x * (Helper::Random() + x);
				Vector3::Type ty = 1.0f - rcpDimension.y * (Helper::Random() + currLine);

				camera.CalculateRay(tx, ty, ray);

				Vector3 color;
				EvaluateColor(color, ray, camera.GetNearPlane(), FLT_MAX, maxRaycastDepth);
				finalColor += color;
			}

			SetPixel(x, currLine, finalColor);
		}
		else
		{
			Vector3::Type tx = rcpDimension.x * x;
			Vector3::Type ty = 1.0f - rcpDimension.y * currLine;

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
			finalColor += Vector3(0.5f, 0.5f, 0.5f);

			SetPixelDirect(x, currLine, finalColor);
		}
	}
	++currLine;
}

//----------------------------------------------------------------------------------------------------------------------------------------
void Raytracer::EvaluateColor(Vector3& _color, const Ray& _ray, Vector3::Type _tMin, Vector3::Type _tMax, int depth)
{
	if (depth > 0)
	{
		HitInfo hitInfo;
		RaycastObjects(hitInfo, _ray, _tMin, _tMax);
		if (hitInfo.isHit)
		{
			Ray scattered;
			Vector3 attenuation;
			if (hitInfo.material->Scatter(_ray, hitInfo, attenuation, scattered))
			{
				Vector3 nextColor;
				EvaluateColor(nextColor, scattered, 0.001, DBL_MAX, depth - 1);
				_color = attenuation * nextColor;
			}
			else
			{
				_color = Vector3(0,0,0);
			}
		}
		else
		{
			SampleEnviroment(_color, _ray.direction);
		}
	}
	else
	{
		_color = Vector3(0,0,0);
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
const Vector3 colorZenith = { 0.6f, 0.6f, 1.0f };
const Vector3 colorHorizon = { 0.8f, 0.8f, 1.0f };
const Vector3 colorGround = { 0.5f, 0.5f, 0.5f };

const Vector3 sunDirection = { -0.38f, -0.64f, 0.38f };
const float sunFocus = 4.0f;
const float sunIntensity = 1.0f;

//----------------------------------------------------------------------------------------------------------------------------------------
void Raytracer::SampleEnviroment(float* _color, const float* _rayDirection)
{
	float skyGradientT = powf(FastMath::SmwoothStep(0.0f, 0.4f, _rayDirection[1]), 0.35f);

	Vector3 skyGradient;
	Vec3_Lerp(skyGradient, colorHorizon, colorZenith, skyGradientT);
	
	float sun = powf(FastMath::Max(0.0f, -Vec3_Dot(_rayDirection, sunDirection)), sunFocus) * sunIntensity;

	float groundToSkyT = FastMath::SmwoothStep(0.0f, 0.01f, _rayDirection[1]);
	float sunMask = groundToSkyT >= 1;
	Vec3_Lerp(_color, colorGround, skyGradient, groundToSkyT);

	Vector3 sunColor;
	Vec3_Set(sunColor,1,1,1);
	Vec3_Mul(sunColor, sun * sunMask);
	Vec3_Add(_color, sunColor);
}
*/

//----------------------------------------------------------------------------------------------------------------------------------------
const Vector3 colorA = Vector3(1.0, 1.0, 1.0);
const Vector3 colorB = Vector3(0.5, 0.7, 1.0);

//----------------------------------------------------------------------------------------------------------------------------------------
void Raytracer::SampleEnviroment(Vector3& _color, const Vector3& _rayDirection)
{
	Vector3::Type t = 0.5 + 0.5 * _rayDirection.y;
	_color.Lerp(colorA, colorB, t);
}

//----------------------------------------------------------------------------------------------------------------------------------------
void Raytracer::SetPixel(int _x, int _y, const Vector3& _color)
{
	float scale = 1.0f / samplesPerPixel;

	unsigned char bR = static_cast<unsigned char>(sqrt(_color.x * scale) * 255.0f);
	unsigned char bG = static_cast<unsigned char>(sqrt(_color.y * scale) * 255.0f);
	unsigned char bB = static_cast<unsigned char>(sqrt(_color.z * scale) * 255.0f);

	UINT32* pixel = static_cast<UINT32*>(imageBuffer);
	pixel[_x + _y * imageWidth] = TO_RGB(bR, bG, bB);
}

//----------------------------------------------------------------------------------------------------------------------------------------
void Raytracer::SetPixelDirect(int _x, int _y, const Vector3& _color)
{
	unsigned char bR = static_cast<unsigned char>(_color.x * 255.0f);
	unsigned char bG = static_cast<unsigned char>(_color.y * 255.0f);
	unsigned char bB = static_cast<unsigned char>(_color.z * 255.0f);

	UINT32* pixel = static_cast<UINT32*>(imageBuffer);
	pixel[_x + _y * imageWidth] = TO_RGB(bR, bG, bB);
}
