#pragma once

#include "Camera.h"
#include <vector>
#include <mutex>
#include <thread>

//----------------------------------------------------------------------------------------------------------------------------------------
class World;
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
	bool			PollFinishedFlag()				{ bool flag = isFinished; isFinished = false; return flag; }
	
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
	bool			useEnviromentBackground;
	World*			world;

	//------------------------------------------------------------------------------------------------------------------------------------
	unsigned int	maxRenderThreads;
	typedef std::vector<std::thread*> RenderThreads;
	RenderThreads	renderThreads;

	std::mutex		mutexRemainingLines;
	std::vector<int> remainingLines;

	bool*			renderFinished;
	std::mutex		mutexRenderThreads;

	volatile bool	isRunning;
	bool			isFinished;

	//------------------------------------------------------------------------------------------------------------------------------------
	double			rcpTimerFreq;
	LARGE_INTEGER	timeStamp;

private:
	//------------------------------------------------------------------------------------------------------------------------------------
	void			CleanupThreads();
	void			SetPixel(int _x, int _y, const Color& _color);
	
	//------------------------------------------------------------------------------------------------------------------------------------
	void			TraceScene(int _threadIndex);
	Color			EvaluateColor(const Ray& _ray, Vector3::Type _tMin, Vector3::Type _tMax, int depth);
	Color			SampleEnviroment(const Vector3& _rayDirection);
};
