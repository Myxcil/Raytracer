#pragma once

//----------------------------------------------------------------------------------------------------------------------------------------
class PDF;
class Camera;
class SceneNode;
struct HitInfo;

//----------------------------------------------------------------------------------------------------------------------------------------
#include "Objects/TraceableObject.h"

//----------------------------------------------------------------------------------------------------------------------------------------
class World
{
public:
	//------------------------------------------------------------------------------------------------------------------------------------
	World();
	~World();

	//------------------------------------------------------------------------------------------------------------------------------------
	void				Init(Camera& _camera, Color& _backGround);
	void				Raycast(HitInfo& _hitInfo, const Ray& _ray, Vector3::Type _tMin, Vector3::Type _tMax);

	PDF*				GetLightsPDF() const { return lightsPDF; }

private:
	//------------------------------------------------------------------------------------------------------------------------------------
	void				InitCornellBox(Camera& _camera, Color& _backGround);
	void				InitTestscene(Camera& _camera, Color& _backGround);
	void				InitTeapot(Camera& _camera, Color& _backGround);

private:
	//------------------------------------------------------------------------------------------------------------------------------------
	TraceableObjects	worldObjects;
	bool				useAABB;
	const SceneNode*	root;
	TraceableObjects	singleObjects;

	TraceableObjects	lights;
	PDF*				lightsPDF;
};