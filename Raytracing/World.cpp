#include "pch.h"
#include "World.h"

#include "Camera.h"
#include "SceneNode.h"
#include "HitInfo.h"

#include "Materials/Texture.h"
#include "Materials/Material.h"

#include "Objects/Sphere.h"
#include "Objects/InfinitePlane.h"
#include "Objects/Quad.h"
#include "Objects/Box.h"
#include "Objects/TriangleMesh.h"

//----------------------------------------------------------------------------------------------------------------------------------------
World::World() :
	useAABB(true),
	root(nullptr),
	useEnviromentBackground(false),
	backGround(0,0,0)
{
}

//----------------------------------------------------------------------------------------------------------------------------------------
World::~World()
{
	lights.clear();
	singleObjects.clear();
	
	for (size_t i = 0; i < worldObjects.size(); ++i)
	{
		delete worldObjects[i];
	}
	worldObjects.clear();
}

//----------------------------------------------------------------------------------------------------------------------------------------
void World::Init(Camera& _camera)
{
	InitCornellBox(_camera);
	//InitTestscene(_camera);
	//InitTeapot(_camera);

	if (useAABB)
	{
		TraceableObjects objectsWithAABB;
		for (size_t i = 0; i < worldObjects.size(); ++i)
		{
			if (worldObjects[i]->GetAABB().IsValid())
			{
				objectsWithAABB.push_back(worldObjects[i]);
			}
			else
			{
				singleObjects.push_back(worldObjects[i]);
			}
		}
		if (objectsWithAABB.size() > 0)
		{
			root = new SceneNode(objectsWithAABB, 0, objectsWithAABB.size(), 0);
		}
	}
	else
	{
		for (size_t i = 0; i < worldObjects.size(); ++i)
		{
			singleObjects.push_back(worldObjects[i]);
		}
	}

	for (size_t i = 0; i < worldObjects.size(); ++i)
	{
		if (worldObjects[i]->IsEmissive())
		{
			lights.push_back(worldObjects[i]);
		}
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------
void World::InitCornellBox(Camera& _camera)
{
	useEnviromentBackground = false;

	_camera.SetPosition(Vector3(0, 0.0, -3));

	// cornell box
	Material* matRed = new LambertMaterial(&ConstantColor::RED);
	Material* matGreen = new LambertMaterial(&ConstantColor::GREEN);
	Material* matWhite = new LambertMaterial(&ConstantColor::WHITE);

	// actual box
	worldObjects.push_back(new Quad(Vector3(0, -1, 0), Vector3(0, 1, 0), Vector3(1, 1, 0), matWhite));	// bottom
	worldObjects.push_back(new Quad(Vector3(0, 0, 1), Vector3(0, 0, -1), Vector3(1, 1, 0), matWhite));	// back
	worldObjects.push_back(new Quad(Vector3(0, 1, 0), Vector3(0, -1, 0), Vector3(1, 1, 0), matWhite));	// top
	worldObjects.push_back(new Quad(Vector3(0, 0, -1), Vector3(0, 0, 1), Vector3(1, 1, 0), matWhite));	// front

	worldObjects.push_back(new Quad(Vector3(-1, 0, 0), Vector3(1, 0, 0), Vector3(1, 1, 0), matRed));	// left
	worldObjects.push_back(new Quad(Vector3(1, 0, 0), Vector3(-1, 0, 0), Vector3(1, 1, 0), matGreen));	// right

	// light
	double lightIntensity = 5.0f;
	Material* lightWhite = new DiffuseLight(Color::ONE * lightIntensity);
	worldObjects.push_back(new Quad(Vector3(0, 0.99, 0), Vector3(0, -1, 0), Vector3(0.25, 0.25, 0), lightWhite));

	// test objects
	Material* matBlue = new LambertMaterial(&ConstantColor::BLUE);
	worldObjects.push_back(new Sphere(Vector3(0, -0.5, 0.25), 0.5, matBlue));
	/**/
	Material* matGlass = new DielectricMaterial(1.5f);
	worldObjects.push_back(new Sphere(Vector3(-0.5, -0.75, -0.5), 0.25, matGlass));
	/**/
	Material* matMetal = new MetalMaterial(&ConstantColor::WHITE, 0);
	worldObjects.push_back(new Sphere(Vector3(0.5, -0.75, -0.5), 0.25, matMetal));
	/**/
}

//----------------------------------------------------------------------------------------------------------------------------------------
void World::InitTestscene(Camera& _camera)
{
	useEnviromentBackground = true;

	_camera.SetPosition(Vector3(5, 4, -5));
	_camera.LookAt(Vector3(0, 1, 0));

	Material* matGrey = new LambertMaterial(&ConstantColor::GREY);
	worldObjects.push_back(new InfinitePlane(Vector3(0, 0, 0), Vector3(0, 1, 0), true, matGrey));

	Texture* checkerboard = new CheckerTexture(&ConstantColor::BLACK, &ConstantColor::WHITE, Vector3(4, 4, 4));
	Material* matCheckerboard = new LambertMaterial(checkerboard);
	worldObjects.push_back(new Sphere(Vector3(0, 1, -1), 1, matCheckerboard));

	//Material* matRed = new MetalMaterial(&ConstantColor::RED, 0);
	Material* matRed = new LambertMaterial(&ConstantColor::RED);
	worldObjects.push_back(new Sphere(Vector3(-3, 2, -3), 2, matRed));

	Material* matGreen = new LambertMaterial(&ConstantColor::GREEN);
	worldObjects.push_back(new Sphere(Vector3(-2.5, 1, 3), 1, matGreen));

	Material* matBlue = new LambertMaterial(&ConstantColor::BLUE);
	worldObjects.push_back(new Sphere(Vector3(2, 1, 0), 1, matBlue));

	Material* lightWhite = new DiffuseLight(Color(10, 10, 10));
	worldObjects.push_back(new Sphere(Vector3(0, 10, 0), 1, lightWhite));
}

//----------------------------------------------------------------------------------------------------------------------------------------
void World::InitTeapot(Camera& _camera)
{
	useEnviromentBackground = true;

	_camera.SetPosition(Vector3(3, 4, -6));
	_camera.LookAt(Vector3(0, 1, 0));

	Material* matGrey = new LambertMaterial(&ConstantColor::GREY);
	worldObjects.push_back(new InfinitePlane(Vector3(0, 0, 0), Vector3(0, 1, 0), true, matGrey));

	Material* matGreen = new LambertMaterial(&ConstantColor::GREEN);
	worldObjects.push_back(new TriangleMesh(Vector3(0,0,0), _T("Data/teapot.obj"), 1.0f, matGreen));
}

//----------------------------------------------------------------------------------------------------------------------------------------
void World::Raycast(HitInfo& _hitInfo, const Ray& _ray, double _tMin, double _tMax)
{
	if (root != nullptr)
	{
		root->Raycast(_hitInfo, _ray, _tMin, _tMax);
	}
	
	HitInfo singleHit;
	for (size_t i = 0; i < singleObjects.size(); ++i)
	{
		singleObjects[i]->Raycast(singleHit, _ray, _tMin, _tMax);
		if (singleHit.isHit && singleHit.distance < _hitInfo.distance)
		{
			_hitInfo = singleHit;
		}
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------
const Color colorA = Color(1.0, 1.0, 1.0);
const Color colorB = Color(0.5, 0.7, 1.0);

//----------------------------------------------------------------------------------------------------------------------------------------
Color World::SampleEnviroment(const Vector3& _rayDirection) const
{
	if (useEnviromentBackground)
	{
		double t = 0.5 + 0.5 * _rayDirection.y;
		return Color::Lerp(colorA, colorB, t);
	}
	else
	{
		return backGround;
	}
}
