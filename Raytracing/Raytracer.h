#pragma once

#include "Camera.h"
#include <vector>
#include <mutex>
#include <thread>

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
	void			Resize(int _width, int _height);
	void			Run();
	bool			IsRunning();
	const void*		GetResult() const				{ return imageBuffer; }
	
private:
	//------------------------------------------------------------------------------------------------------------------------------------
	void*			imageBuffer;
	int				imageWidth;
	int				imageHeight;

	//------------------------------------------------------------------------------------------------------------------------------------
	Camera			camera;
	int				currLine;
	Vector3			rcpDimension;
	int				samplesPerPixel;
	int				maxRaycastDepth;
	Color			backGround;

	//------------------------------------------------------------------------------------------------------------------------------------
	typedef std::vector<TraceableObject*> TraceableObjects;
	TraceableObjects traceableObjects;

	//------------------------------------------------------------------------------------------------------------------------------------
	typedef std::vector<std::thread*> RenderThreads;
	RenderThreads	renderThreads;
	bool*			renderFinished;
	std::mutex		mutexRenderThreads;
	volatile bool	isRunning;

	//------------------------------------------------------------------------------------------------------------------------------------
	double			rcpTimerFreq;
	LARGE_INTEGER	timeStamp;

private:
	//------------------------------------------------------------------------------------------------------------------------------------
	void		InitScene();
	void		SetPixel(int _x, int _y, const Color& _color);
	
	void		TraceScene(int _threadIndex, int _startLine, int _numLines);
	Color		EvaluateColor(const Ray& _ray, const Color& _backGround, Vector3::Type _tMin, Vector3::Type _tMax, int depth);
	void		RaycastObjects(HitInfo& _hitInfo, const Ray& _ray, Vector3::Type _tMin, Vector3::Type _tMax);
	Color		SampleEnviroment(const Vector3& _rayDirection);

	//------------------------------------------------------------------------------------------------------------------------------------
	void		InitCornellBox();
	void		CleanupThreads();
};
