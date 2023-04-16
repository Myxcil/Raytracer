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
	void			SetPixel(int _x, int _y, UINT32 _color);
	
	//------------------------------------------------------------------------------------------------------------------------------------
	void			TraceScene(int _threadIndex);
	Color			EvaluateColor(const Ray& _ray, double _tMin, double _tMax, int _depth, Vector3 _throughput);
	void			EvaluatePDF(HitInfo& _hitInfo);

	//------------------------------------------------------------------------------------------------------------------------------------
	bool			PlotImage();
};
