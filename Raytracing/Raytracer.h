#pragma once

#include "Camera.h"
#include <vector>

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
	Vector3		rcpDimension;
	int			samplesPerPixel;
	int			maxRaycastDepth;

	typedef std::vector<TraceableObject*> TraceableObjects;
	TraceableObjects traceableObjects;

private:
	//------------------------------------------------------------------------------------------------------------------------------------
	void		InitScene();
	void		SetPixel(int _x, int _y, const Color& _color);
	void		SetPixelDirect(int _x, int _y, const Color& _color);
	
	void		TraceScene(int _line);
	void		EvaluateColor(Color& _color, const Ray& _ray, Vector3::Type _tMin, Vector3::Type _tMax, int depth);
	void		RaycastObjects(HitInfo& _hitInfo, const Ray& _ray, Vector3::Type _tMin, Vector3::Type _tMax);
	void		SampleEnviroment(Color& _color, const Vector3& _rayDirection);
};
