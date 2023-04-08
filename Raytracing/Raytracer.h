#pragma once

#include "Camera.h"
#include <vector>

using namespace DirectX::SimpleMath;

//----------------------------------------------------------------------------------------------------------------------------------------
class TraceableObject;
struct HitInfo;

//----------------------------------------------------------------------------------------------------------------------------------------
class Raytracer
{
public:
	//------------------------------------------------------------------------------------------------------------------------------------
	Raytracer();
	~Raytracer();

	//------------------------------------------------------------------------------------------------------------------------------------
	void		Resize(int _width, int _height);
	void		Update(float _deltaTime);
	const void*	GetResult() const				{ return imageBuffer; }
	
private:
	//------------------------------------------------------------------------------------------------------------------------------------
	void*		imageBuffer;
	int			imageWidth;
	int			imageHeight;
	int			linesPerUpdate;

	//------------------------------------------------------------------------------------------------------------------------------------
	Camera		camera;
	int			currLine;
	Vector2		rcpDimension;
	int			samplesPerPixel;
	int			maxRaycastDepth;

	typedef std::vector<TraceableObject*> TraceableObjects;
	TraceableObjects traceableObjects;

private:
	//------------------------------------------------------------------------------------------------------------------------------------
	void		InitScene();
	void		SetPixel(int _x, int _y, const Vector3& _color);
	void		SetPixelDirect(int _x, int _y, const Vector3& _color);
	
	void		TraceScene();
	void		EvaluateColor(Vector3& _color, const Vector3& _rayOrigin, const Vector3& _rayDirection, float _tMin, float _tMax, unsigned int& _randomState, int depth);
	void		RaycastObjects(HitInfo& _hitInfo, const Vector3& _rayOrigin, const Vector3& _rayDirection, float _tMin, float _tMax);
	void		SampleEnviroment(Vector3& _color, const Vector3& _rayDirection);
};
