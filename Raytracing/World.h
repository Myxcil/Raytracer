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
	void				Init(Camera& _camera);
	void				Raycast(HitInfo& _hitInfo, const Ray& _ray, double _tMin, double _tMax);
	Color				SampleEnviroment(const Vector3& _rayDirection) const;

private:
	//------------------------------------------------------------------------------------------------------------------------------------
	void				InitCornellBox(Camera& _camera);
	void				InitTestscene(Camera& _camera);
	void				InitTeapot(Camera& _camera);


private:
	//------------------------------------------------------------------------------------------------------------------------------------
	TraceableObjects	worldObjects;
	bool				useAABB;
	const SceneNode*	root;
	TraceableObjects	singleObjects;

	Color				backGround;
	bool				useEnviromentBackground;
};