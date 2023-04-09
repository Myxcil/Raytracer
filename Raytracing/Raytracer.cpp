#include "pch.h"
#include "Raytracer.h"
#include "World.h"
#include "Objects/TraceableObject.h"
#include "Material.h"

//----------------------------------------------------------------------------------------------------------------------------------------
Raytracer::Raytracer() :
	imageBuffer(nullptr),
	imageWidth(0),
	imageHeight(0),
	rcpDimension(0,0,0),
	currLine(0),
	samplesPerPixel(10),
	maxRaycastDepth(8),
	backGround(0,0,0),
	useEnviromentBackground(false),
	maxRenderThreads(1),
	isRunning(false),
	isFinished(false)
{
	LARGE_INTEGER freq;
	QueryPerformanceFrequency(&freq);
	rcpTimerFreq = 1.0 / freq.QuadPart;

	world = new World();
	world->Init(camera, useEnviromentBackground);
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
	if (isRunning)
		return;

	QueryPerformanceCounter(&timeStamp);

	isRunning = true;
	isFinished = false;

	unsigned int numCores = std::thread::hardware_concurrency();
	unsigned int numThreads = max(1,numCores - 2);
	if (maxRenderThreads > 0)
	{
		numThreads = min(numThreads, maxRenderThreads);
	}

	float linesPerThread = static_cast<float>(imageHeight) / numThreads;

	Helper::Log(_T("Raytracing start: %d threads, with %f lines/thread\n"), numThreads, linesPerThread);

	int lineCount = static_cast<int>(linesPerThread + 0.5f);
	renderFinished = new bool[lineCount];

	int linesRemain = imageHeight;
	int startLine = 0;
	for (unsigned int i = 0; i < numThreads; ++i)
	{
		int numLines = min(linesRemain, lineCount);

		renderFinished[i] = false;

		std::thread* newThread = new std::thread(&Raytracer::TraceScene, this, i, startLine, numLines);
		renderThreads.emplace_back(newThread);

		linesRemain -= numLines;
		startLine += numLines;
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
			Helper::Log(_T("Rendering time total: %f\n"), duration);
	
			CleanupThreads();

			isFinished = true;
		}
	}
	return isRunning;
}

//----------------------------------------------------------------------------------------------------------------------------------------
void Raytracer::TraceScene(int _threadIndex, int _startLine, int _numLines)
{
	Helper::Log(_T("Launch thread #%d with (%d - %d)\n"), _threadIndex, _startLine, _startLine+_numLines);

	Ray ray;
	for(int i = 0; i < _numLines && isRunning; ++i)
	{
		int y = _startLine + i;
		for (int x = 0; x < imageWidth && isRunning; ++x)
		{
			Color finalColor;
			for (int i = 0; i < samplesPerPixel; ++i)
			{
				Vector3::Type tx = rcpDimension.x * (Helper::Random() + x);
				Vector3::Type ty = 1.0f - rcpDimension.y * (Helper::Random() + y);

				camera.CalculateRay(tx, ty, ray);

				finalColor += EvaluateColor(ray, camera.GetNearPlane(), FLT_MAX, maxRaycastDepth);
			}

			SetPixel(x, y, finalColor);
		}
	}

	LARGE_INTEGER endTime;
	QueryPerformanceCounter(&endTime);

	double duration = rcpTimerFreq * (endTime.QuadPart - timeStamp.QuadPart);
	Helper::Log(_T("Thread #%d finished after %f\n"), _threadIndex, duration);

	mutexRenderThreads.lock();
	{
		renderFinished[_threadIndex] = true;
	}
	mutexRenderThreads.unlock();
}

//----------------------------------------------------------------------------------------------------------------------------------------
Color Raytracer::EvaluateColor(const Ray& _ray, Vector3::Type _tMin, Vector3::Type _tMax, int depth)
{
	if (depth <= 0)
		return Color(0,0,0);

	HitInfo hitInfo;
	world->Raycast(hitInfo, _ray, _tMin, _tMax);
	if (!hitInfo.isHit)
	{
		return SampleEnviroment(_ray.direction);
	}

	Color emitted = hitInfo.material->Emitted(hitInfo.uvw, hitInfo.point);

	Ray scattered;
	Color attenuation;
	if (!hitInfo.material->Scatter(_ray, hitInfo, attenuation, scattered))
		return emitted;

	return emitted + attenuation * EvaluateColor(scattered, 0.001, DBL_MAX, depth - 1);;
}

//----------------------------------------------------------------------------------------------------------------------------------------
const Color colorA = Color(1.0, 1.0, 1.0);
const Color colorB = Color(0.5, 0.7, 1.0);

//----------------------------------------------------------------------------------------------------------------------------------------
Color Raytracer::SampleEnviroment(const Vector3& _rayDirection)
{
	if (useEnviromentBackground)
	{
		Vector3::Type t = 0.5 + 0.5 * _rayDirection.y;
		return Color::Lerp(colorA, colorB, t);
	}
	else
	{
		return backGround;
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------
void Raytracer::SetPixel(int _x, int _y, const Color& _color)
{
	float scale = 1.0f / samplesPerPixel;
	UINT32* pixel = static_cast<UINT32*>(imageBuffer);
	pixel[_x + _y * imageWidth] = _color.ToRGB(scale);
}
