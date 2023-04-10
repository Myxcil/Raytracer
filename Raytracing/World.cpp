#include "pch.h"
#include "World.h"

#include "Camera.h"
#include "SceneNode.h"

#include "Textures/Texture.h"

#include "Material.h"

#include "Objects/Sphere.h"
#include "Objects/InfinitePlane.h"
#include "Objects/Quad.h"
#include "Objects/Box.h"
#include "Objects/TriangleMesh.h"

//----------------------------------------------------------------------------------------------------------------------------------------
World::World() :
	useAABB(true),
	root(nullptr),
	lightsPDF(nullptr)
{
}

//----------------------------------------------------------------------------------------------------------------------------------------
World::~World()
{
	singleObjects.clear();

	for (size_t i = 0; i < worldObjects.size(); ++i)
	{
		delete worldObjects[i];
	}
	worldObjects.clear();
}

//----------------------------------------------------------------------------------------------------------------------------------------
void World::Init(Camera& _camera, Color& _backGround)
{
	InitCornellBox(_camera, _backGround);
	//InitTestscene(_camera, _backGround);
	//InitTeapot(_camera, _backGround);

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

	for(int i=0; i < worldObjects.size(); ++i)
	{
		if (worldObjects[i]->IsEmissive())
		{
			lights.push_back(worldObjects[i]);
		}
	}
	if (lights.size() > 0)
	{
		lightsPDF = new ObjectsPDF(lights);
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------
void World::InitCornellBox(Camera& _camera, Color& _backGround)
{
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
	Material* lightWhite = new DiffuseLight(Color(4, 4, 4));
	worldObjects.push_back(new Quad(Vector3(0, 0.99, 0), Vector3(0, -1, 0), Vector3(0.25, 0.25, 0), lightWhite));

	// test objects
	/**
	Material* matYellow = new LambertMaterial(&ConstantColor::YELLOW);
	worldObjects.push_back(new Sphere(Vector3(0, -0.75, -0.5), 0.25, matYellow));

	Material* matBlue = new LambertMaterial(&ConstantColor::BLUE);
	worldObjects.push_back(new Sphere(Vector3(-0.5, -0.75, 0.25), 0.25, matBlue));

	Material* matGrey = new LambertMaterial(&ConstantColor::GREY);
	worldObjects.push_back(new Sphere(Vector3(0.5, -0.75, 0.25), 0.25, matGrey));
	/**/
}

//----------------------------------------------------------------------------------------------------------------------------------------
void World::InitTestscene(Camera& _camera, Color& _backGround)
{
	_camera.SetPosition(Vector3(5, 4, -5));
	_camera.LookAt(Vector3(0, 1, 0));

	Material* matGrey = new LambertMaterial(&ConstantColor::GREY);
	worldObjects.push_back(new InfinitePlane(Vector3(0, 0, 0), Vector3(0, 1, 0), true, matGrey));

	Texture* checkerboard = new CheckerTexture(&ConstantColor::BLACK, &ConstantColor::WHITE, Vector3(4, 4, 4));
	Material* matCheckerboard = new LambertMaterial(checkerboard);

	Material* matRed = new LambertMaterial(&ConstantColor::RED);
	Material* matGreen = new LambertMaterial(&ConstantColor::GREEN);
	Material* matBlue = new LambertMaterial(&ConstantColor::BLUE);
	worldObjects.push_back(new Sphere(Vector3(0, 1, -1), 1, matCheckerboard));
	worldObjects.push_back(new Sphere(Vector3(-2, 1, 1), 1, matGreen));
	worldObjects.push_back(new Sphere(Vector3(2, 1, 1), 1, matBlue));
	worldObjects.push_back(new Sphere(Vector3(-3, 2, -3), 2, matRed));

	Material* lightWhite = new DiffuseLight(Color(4, 4, 4));
	worldObjects.push_back(new Sphere(Vector3(0, 5, 0), 1, lightWhite));
}

//----------------------------------------------------------------------------------------------------------------------------------------
void World::InitTeapot(Camera& _camera, Color& _backGround)
{
	_camera.SetPosition(Vector3(3, 4, -6));
	_camera.LookAt(Vector3(0, 1, 0));

	Material* matGrey = new LambertMaterial(&ConstantColor::GREY);
	worldObjects.push_back(new InfinitePlane(Vector3(0, 0, 0), Vector3(0, 1, 0), true, matGrey));

	Material* matGreen = new LambertMaterial(&ConstantColor::GREEN);
	worldObjects.push_back(new TriangleMesh(Vector3(0,0,0), _T("Data/teapot.obj"), 1.0f, matGreen));
	//worldObjects.push_back(new TriangleMesh(Vector3(0,2,0), nullptr, matGreen));
}

//----------------------------------------------------------------------------------------------------------------------------------------
void World::Raycast(HitInfo& _hitInfo, const Ray& _ray, Vector3::Type _tMin, Vector3::Type _tMax)
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