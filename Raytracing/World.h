#pragma once

//----------------------------------------------------------------------------------------------------------------------------------------
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
	void				Init(Camera& _camera, bool& _useEnviromentLight);
	void				Raycast(HitInfo& _hitInfo, const Ray& _ray, Vector3::Type _tMin, Vector3::Type _tMax);

private:
	//------------------------------------------------------------------------------------------------------------------------------------
	void				InitCornellBox(Camera& _camera, bool& _useEnviromentLight);
	void				InitTestscene(Camera& _camera, bool& _useEnviromentLight);
	void				InitTeapot(Camera& _camera, bool& _useEnviromentLight);

private:
	//------------------------------------------------------------------------------------------------------------------------------------
	TraceableObjects	worldObjects;
	bool				useAABB;
	const SceneNode*	root;
	TraceableObjects	singleObjects;
};