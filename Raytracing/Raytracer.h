#pragma once

#include "Camera.h"
#include <vector>
#include <mutex>
#include <thread>

//----------------------------------------------------------------------------------------------------------------------------------------
class World;
class PDF;
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
	
	World*			world;
	PDF*			lightPDF;

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
	Color			EvaluateColor(const Ray& _ray, int depth);
	
	//------------------------------------------------------------------------------------------------------------------------------------
	Vector3::Type	GetPDFValue(const PDF& _pdf, const Vector3& _direction) const;
	Vector3			GeneratePDFDirection(const PDF& _pdf) const;
};
