#include "pch.h"
#include "World.h"

#include "Camera.h"
#include "SceneNode.h"

#include "Textures/Texture.h"

#include "Material.h"

#include "Objects/Sphere.h"
#include "Objects/InfinitePlane.h"
#include "Objects/Quad.h"

//----------------------------------------------------------------------------------------------------------------------------------------
World::World() :
	useAABB(true),
	root(nullptr)
{
}

//----------------------------------------------------------------------------------------------------------------------------------------
World::~World()
{
	singleObjects.clear();

	for (size_t i = 0; i < traceableObjects.size(); ++i)
	{
		delete traceableObjects[i];
	}
	traceableObjects.clear();
}

//----------------------------------------------------------------------------------------------------------------------------------------
void World::Init(Camera& _camera, bool& _useEnviromentLight)
{
	//InitCornellBox(_camera, _useEnviromentLight);
	InitTestscene(_camera, _useEnviromentLight);

	if (useAABB)
	{
		TraceableObjects objectsWithAABB;
		for (size_t i = 0; i < traceableObjects.size(); ++i)
		{
			if (traceableObjects[i]->GetAABB().IsValid())
			{
				objectsWithAABB.push_back(traceableObjects[i]);
			}
			else
			{
				singleObjects.push_back(traceableObjects[i]);
			}
		}
		root = new SceneNode(objectsWithAABB, 0, objectsWithAABB.size(), 0);
	}
	else
	{
		for (size_t i = 0; i < traceableObjects.size(); ++i)
		{
			singleObjects.push_back(traceableObjects[i]);
		}
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------
void World::InitCornellBox(Camera& _camera, bool& _useEnviromentLight)
{
	_useEnviromentLight = false;

	_camera.SetPosition(Vector3(0, 0.0, -3));

	// cornell box
	Material* matRed = new LambertMaterial(&ConstantColor::RED);
	Material* matGreen = new LambertMaterial(&ConstantColor::GREEN);
	Material* matWhite = new LambertMaterial(&ConstantColor::WHITE);

	// actual box
	traceableObjects.push_back(new Quad(Vector3(0, -1, 0), Vector3(0, 1, 0), Vector3(1, 1, 0), matWhite));	// bottom
	traceableObjects.push_back(new Quad(Vector3(0, 0, 1), Vector3(0, 0, -1), Vector3(1, 1, 0), matWhite));	// back
	traceableObjects.push_back(new Quad(Vector3(0, 1, 0), Vector3(0, -1, 0), Vector3(1, 1, 0), matWhite));	// top
	traceableObjects.push_back(new Quad(Vector3(0, 0, -1), Vector3(0, 0, 1), Vector3(1, 1, 0), matWhite));	// front

	traceableObjects.push_back(new Quad(Vector3(-1, 0, 0), Vector3(1, 0, 0), Vector3(1, 1, 0), matRed));	// left
	traceableObjects.push_back(new Quad(Vector3(1, 0, 0), Vector3(-1, 0, 0), Vector3(1, 1, 0), matGreen));	// right

	// light
	Material* lightWhite = new DiffuseLight(Color(4, 4, 4));
	traceableObjects.push_back(new Quad(Vector3(0, 0.99, 0), Vector3(0, -1, 0), Vector3(0.25, 0.25, 0), lightWhite));

	// test objects
	Material* matGlass = new DielectricMaterial(1.5f);
	traceableObjects.push_back(new Sphere(Vector3(0, -0.75, -0.5), 0.25, matGlass));

	Material* matBlue = new LambertMaterial(&ConstantColor::BLUE);
	traceableObjects.push_back(new Sphere(Vector3(-0.5, -0.75, 0.25), 0.25, matBlue));

	Material* matMetal = new MetalMaterial(&ConstantColor::WHITE, 0);
	traceableObjects.push_back(new Sphere(Vector3(0.5, -0.75, 0.25), 0.25, matMetal));
}

//----------------------------------------------------------------------------------------------------------------------------------------
void World::InitTestscene(Camera& _camera, bool& _useEnviromentLight)
{
	_useEnviromentLight = true;

	_camera.SetPosition(Vector3(5, 4, -5));
	_camera.LookAt(Vector3(0, 1, 0));

	Material* matGrey = new LambertMaterial(&ConstantColor::GREY);
	traceableObjects.push_back(new InfinitePlane(Vector3(0, 0, 0), Vector3(0, 1, 0), true, matGrey));

	Texture* checkerboard = new CheckerTexture(&ConstantColor::BLACK, &ConstantColor::WHITE, Vector3(4, 4, 4));
	Material* matCheckerboard = new LambertMaterial(checkerboard);

	Material* matRed = new MetalMaterial(&ConstantColor::RED, 0.05);
	Material* matGreen = new LambertMaterial(&ConstantColor::GREEN);
	Material* matBlue = new LambertMaterial(&ConstantColor::BLUE);
	traceableObjects.push_back(new Sphere(Vector3(0, 1, -1), 1, matCheckerboard));
	traceableObjects.push_back(new Sphere(Vector3(-2, 1, 1), 1, matGreen));
	traceableObjects.push_back(new Sphere(Vector3(2, 1, 1), 1, matBlue));
	traceableObjects.push_back(new Sphere(Vector3(-3, 2, -3), 2, matRed));

	Material* lightWhite = new DiffuseLight(Color(4, 4, 4));
	traceableObjects.push_back(new Sphere(Vector3(0, 5, 0), 1, lightWhite));
}

//----------------------------------------------------------------------------------------------------------------------------------------
void World::Raycast(HitInfo& _hitInfo, const Ray& _ray, Vector3::Type _tMin, Vector3::Type _tMax)
{
	_hitInfo.isHit = false;
	_hitInfo.distance = DBL_MAX;

	if (root != nullptr)
	{
		root->Raycast(_hitInfo, _ray, _tMin, _tMax);
	}
	for (size_t i = 0; i < singleObjects.size(); ++i)
	{
		singleObjects[i]->Raycast(_hitInfo, _ray, _tMin, _tMax);
	}
}