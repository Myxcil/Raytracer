#include "pch.h"
#include "Raytracer.h"
#include "World.h"
#include "Objects/TraceableObject.h"
#include "Materials/Material.h"

//----------------------------------------------------------------------------------------------------------------------------------------
Raytracer::Raytracer() :
	imageBuffer(nullptr),
	imageWidth(0),
	imageHeight(0),
	rcpDimension(0,0,0),
	currLine(0),
	samplesPerPixel(100),
	maxRaycastDepth(0),
	maxRenderThreads(0),
	isRunning(false),
	isFinished(false),
	renderFinished(nullptr)
{
	LARGE_INTEGER freq;
	QueryPerformanceFrequency(&freq);
	rcpTimerFreq = 1.0 / freq.QuadPart;

	timeStamp.QuadPart = 0;

	world = new World();
	world->Init(camera);
}

//----------------------------------------------------------------------------------------------------------------------------------------
Raytracer::~Raytracer()
{
	CleanupThreads();

	if (imageBuffer != nullptr)
	{
		delete[] imageBuffer;
		imageBuffer = nullptr;
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------
void Raytracer::CleanupThreads()
{
	isRunning = false;
	if (renderThreads.size() > 0)
	{
		for (size_t i = 0; i < renderThreads.size(); ++i)
		{
			if (renderThreads[i]->joinable())
			{
				renderThreads[i]->join();
			}
			delete renderThreads[i];
		}
		renderThreads.clear();
		Helper::Log(_T("Cleaned render threads\n"));
	}
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
void Raytracer::Run()
{
	if (PlotImage())
	{
		isFinished = true;
		return;
	}

	if (isRunning)
		return;

	QueryPerformanceCounter(&timeStamp);

	isRunning = true;
	isFinished = false;

	unsigned int numCores = std::thread::hardware_concurrency();
	unsigned int numThreads = max(1,numCores - 1);
	if (maxRenderThreads > 0)
	{
		numThreads = min(numThreads, maxRenderThreads);
	}

	Helper::Log(_T("Raytracing start: %d threads\n"), numThreads);

	for(int i=imageHeight-1; i >= 0; --i)
	{
		remainingLines.push_back(i);
	}

	maxRenderThreads = 0;
	renderFinished = new bool[numThreads];
	for (unsigned int i = 0; i < numThreads; ++i)
	{
		renderFinished[i] = false;

		std::thread* newThread = new std::thread(&Raytracer::TraceScene, this, i);
		renderThreads.emplace_back(newThread);
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------
bool Raytracer::IsRunning()
{
	if (isRunning)
	{
		bool allDone = true;
		for (size_t i = 0; i < renderThreads.size(); ++i)
		{
			if (!renderFinished[i])
			{
				allDone = false;
			}
		}
		if (allDone)
		{
			LARGE_INTEGER time;
			QueryPerformanceCounter(&time);
			
			double duration = rcpTimerFreq * (time.QuadPart - timeStamp.QuadPart);
			Helper::Log(_T("Rendering time total %f, max depth %d\n"), duration, maxRaycastDepth);
	
			CleanupThreads();

			isFinished = true;
		}
	}
	return isRunning;
}

//----------------------------------------------------------------------------------------------------------------------------------------
void Raytracer::TraceScene(int _threadIndex)
{
	//Helper::Log(_T("Launched thread #%d)\n"), _threadIndex);

	while (true)	
	{
		int y = -1;
		mutexRemainingLines.lock();
		if (remainingLines.size() > 0)
		{
			y = remainingLines.back();
			remainingLines.pop_back();
		}
		mutexRemainingLines.unlock();
		if (y == -1)
			break;

		Ray ray;
		for (int x = 0; x < imageWidth && isRunning; ++x)
		{
			Color finalColor(0,0,0);
			for (int i = 0; i < samplesPerPixel; ++i)
			{
				double tx = rcpDimension.x * (Helper::Random() + x);
				double ty = 1.0f - rcpDimension.y * (Helper::Random() + y);

				camera.CalculateRay(tx, ty, ray);

				finalColor += EvaluateColor(ray, camera.GetNearPlane(), DBL_MAX, 1, Color(1,1,1));
			}

			SetPixel(x, y, finalColor);
		}
	}

	/*
	LARGE_INTEGER endTime;
	QueryPerformanceCounter(&endTime);
	double duration = rcpTimerFreq * (endTime.QuadPart - timeStamp.QuadPart);
	Helper::Log(_T("Thread #%d finished after %f\n"), _threadIndex, duration);
	*/
	mutexRenderThreads.lock();
	{
		renderFinished[_threadIndex] = true;
	}
	mutexRenderThreads.unlock();
}

//----------------------------------------------------------------------------------------------------------------------------------------
const int MAX_DEPTH = 32;

//----------------------------------------------------------------------------------------------------------------------------------------
Color Raytracer::EvaluateColor(const Ray& _ray, double _tMin, double _tMax, int _depth, Vector3 _throughput)
{
	if (_depth >= MAX_DEPTH)
	{
		return Color::ZERO;
	}

	maxRaycastDepth = max(maxRaycastDepth, _depth);

	HitInfo hitInfo;
	world->Raycast(hitInfo, _ray, _tMin, _tMax);
	if (!hitInfo.isHit)
	{
		return world->SampleEnviroment(_ray.direction);
	}

	Color emitted = hitInfo.material->Emitted(hitInfo.uvw, hitInfo.point);

	ScatterInfo scatterInfo;
	if (!hitInfo.material->Scatter(_ray, hitInfo, scatterInfo))
	{
		return emitted;
	}

	// 'Russian Roulette' ray termination,
	// when accumulated light (throughput) gets "too low".
	// Each iteration which doesn't involve a 100% reflection
	// will reduce the throughput and make it more likely that
	// this ray terminates
	double threshold = fmin(0.999, _throughput.Max());
	if (Helper::Random() >= threshold)
	{
		return emitted;
	}
	
	Ray nextRay = Ray(hitInfo.point, scatterInfo.direction);
	nextRay.origin += nextRay.direction * 0.000001;

	// attenuation from ScatterInfo already contains factors 
	// like cosTheta or PI, calculated in the material
	Vector3 attenuation = scatterInfo.attenuation / threshold;
	_throughput *= attenuation;

	return emitted + attenuation * EvaluateColor(nextRay , 0.001, DBL_MAX, _depth + 1, _throughput);
}

//----------------------------------------------------------------------------------------------------------------------------------------
void Raytracer::SetPixel(int _x, int _y, const Color& _color)
{
	float scale = 1.0f / samplesPerPixel;
	SetPixel(_x, _y, _color.ToRGB(scale));
}

//----------------------------------------------------------------------------------------------------------------------------------------
void Raytracer::SetPixel(int _x, int _y, UINT32 _color)
{
	UINT32* pixel = static_cast<UINT32*>(imageBuffer);
	pixel[_x + _y * imageWidth] = _color;
}

//----------------------------------------------------------------------------------------------------------------------------------------
bool Raytracer::PlotImage()
{
	return false;

	/*
	// show random samples
	int dim = imageHeight;
	int numSamples = 1000;
	int xOff = (imageWidth - dim) >> 1;
	for (int i = 0; i < numSamples; ++i)
	{
		Vector3 v = Helper::RandomDiscXZUniform();
		v += Vector3::ONE;
		v *= 0.5f;
		int px = xOff + round(v.x * dim);
		int py = round(v.z * dim);
		SetPixel(px, py, 0xffffffff);
	}
	*/
}