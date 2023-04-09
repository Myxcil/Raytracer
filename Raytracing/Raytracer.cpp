#include "pch.h"
#include "Raytracer.h"

#include "Material.h"
#include "Objects/TraceableObject.h"
#include "Objects/Sphere.h"
#include "Objects/InfinitePlane.h"
#include "Objects/Quad.h"

//----------------------------------------------------------------------------------------------------------------------------------------
Raytracer::Raytracer() :
	imageBuffer(nullptr),
	imageWidth(0),
	imageHeight(0),
	rcpDimension(0,0,0),
	currLine(0),
	samplesPerPixel(50),
	maxRaycastDepth(8),
	backGround(0,0,0),
	useEnviromentBackground(false),
	useAABB(true),
	isRunning(false),
	isFinished(false)
{
	LARGE_INTEGER freq;
	QueryPerformanceFrequency(&freq);
	rcpTimerFreq = 1.0 / freq.QuadPart;

	InitScene();
}

//----------------------------------------------------------------------------------------------------------------------------------------
Raytracer::~Raytracer()
{
	CleanupThreads();

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
void Raytracer::InitScene()
{
	//InitCornellBox();
	InitTestscene();
}

//----------------------------------------------------------------------------------------------------------------------------------------
void Raytracer::InitCornellBox()
{
	// cornell box
	Material* matRed = new LambertMaterial(Color(1,0,0));
	Material* matGreen = new LambertMaterial(Color(0,1,0));
	Material* matWhite = new LambertMaterial(Color(1,1,1));

	// actual box
	traceableObjects.push_back(new Quad(Vector3(0, -1, 0), Vector3(0, 1, 0), Vector3(1, 1, 0), matWhite));	// bottom
	traceableObjects.push_back(new Quad(Vector3(0, 0, 1), Vector3(0, 0, -1), Vector3(1, 1, 0), matWhite));	// back
	traceableObjects.push_back(new Quad(Vector3(0, 1, 0), Vector3(0, -1, 0), Vector3(1, 1, 0), matWhite));	// top
	traceableObjects.push_back(new Quad(Vector3(0, 0, -1), Vector3(0, 0, 1), Vector3(1, 1, 0), matWhite));	// front

	traceableObjects.push_back(new Quad(Vector3(-1, 0, 0), Vector3(1, 0, 0), Vector3(1, 1, 0), matRed));	// left
	traceableObjects.push_back(new Quad(Vector3(1, 0, 0), Vector3(-1, 0, 0), Vector3(1, 1, 0), matGreen));	// right

	// light
	Material* lightWhite = new DiffuseLight(Color(4,4,4));
	traceableObjects.push_back(new Quad(Vector3(0,0.99,0), Vector3(0,-1,0), Vector3(0.25,0.25,0), lightWhite));	

	// test objects
	Material* matGlass = new DielectricMaterial(1.5f);
	traceableObjects.push_back(new Sphere(Vector3(0,-0.75,-0.5), 0.25, matGlass));

	Material* matBlue = new LambertMaterial(Color(0,0,1));
	traceableObjects.push_back(new Sphere(Vector3(-0.5,-0.75,0.25), 0.25, matBlue));

	Material* matMetal = new MetalMaterial(Color(1,1,1), 0);
	traceableObjects.push_back(new Sphere(Vector3(0.5, -0.75, 0.25), 0.25, matMetal));
}

//----------------------------------------------------------------------------------------------------------------------------------------
void Raytracer::InitTestscene()
{
	useEnviromentBackground = true;

	camera.SetPosition(Vector3(5,4,-5));
	camera.LookAt(Vector3(0,1,0));

	Material* matRed = new LambertMaterial(Color(1, 0, 0));
	Material* matGreen = new LambertMaterial(Color(0, 1, 0));
	Material* matBlue = new LambertMaterial(Color(0, 0, 1));
	Material* matGrey = new LambertMaterial(Color(0.5, 0.5, 0.5));

	traceableObjects.push_back(new InfinitePlane(Vector3(0,0,0), Vector3(0,1,0), true, matGrey));

	traceableObjects.push_back(new Sphere(Vector3(0,1,-1), 1, matRed));
	traceableObjects.push_back(new Sphere(Vector3(-2,1,1), 1, matGreen));
	traceableObjects.push_back(new Sphere(Vector3(2,1,1), 1, matBlue));
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

				Color bg = useEnviromentBackground ? SampleEnviroment(ray.direction) : backGround;

				finalColor += EvaluateColor(ray, bg, camera.GetNearPlane(), FLT_MAX, maxRaycastDepth);
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
Color Raytracer::EvaluateColor(const Ray& _ray, const Color& _backGround, Vector3::Type _tMin, Vector3::Type _tMax, int depth)
{
	if (depth <= 0)
		return Color(0,0,0);

	HitInfo hitInfo;
	RaycastObjects(hitInfo, _ray, _tMin, _tMax);
	if (!hitInfo.isHit)
		return _backGround;

	Ray scattered;
	Color attenuation;
	Color emitted;
	hitInfo.material->Emitted(hitInfo.uvw, hitInfo.point, emitted);
	if (!hitInfo.material->Scatter(_ray, hitInfo, attenuation, scattered))
		return emitted;

	return emitted + attenuation * EvaluateColor(scattered, _backGround, 0.001, DBL_MAX, depth - 1);;
}

//----------------------------------------------------------------------------------------------------------------------------------------
void Raytracer::RaycastObjects(HitInfo& _hitInfo, const Ray& _ray, Vector3::Type _tMin, Vector3::Type _tMax)
{
	_hitInfo.isHit = false;
	_hitInfo.distance = DBL_MAX;

	HitInfo objHit;
	for (size_t i=0; i < traceableObjects.size(); ++i)
	{
		if (!useAABB || traceableObjects[i]->Hit(_ray, _tMin, _tMax))
		{
			traceableObjects[i]->Raycast(objHit, _ray, _tMin, _tMax);
			if (objHit.isHit && objHit.distance < _hitInfo.distance)
			{
				_hitInfo = objHit;
			}
		}
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------
const Color colorA = Color(1.0, 1.0, 1.0);
const Color colorB = Color(0.5, 0.7, 1.0);

//----------------------------------------------------------------------------------------------------------------------------------------
Color Raytracer::SampleEnviroment(const Vector3& _rayDirection)
{
	Vector3::Type t = 0.5 + 0.5 * _rayDirection.y;
	return Color::Lerp(colorA, colorB, t);
}

//----------------------------------------------------------------------------------------------------------------------------------------
void Raytracer::SetPixel(int _x, int _y, const Color& _color)
{
	float scale = 1.0f / samplesPerPixel;
	UINT32* pixel = static_cast<UINT32*>(imageBuffer);
	pixel[_x + _y * imageWidth] = _color.ToRGB(scale);
}
