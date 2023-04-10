#pragma once

#include "Objects/AABB.h"
#include "Objects/TraceableObject.h"

//----------------------------------------------------------------------------------------------------------------------------------------
class SceneNode
{
public:
	//------------------------------------------------------------------------------------------------------------------------------------
	SceneNode(TraceableObjects& _list, size_t _first, size_t _end, int _depth);
	~SceneNode();

	//------------------------------------------------------------------------------------------------------------------------------------
	void					Raycast(HitInfo& _hitInfo, const Ray& _ray, Vector3::Type _tMin, Vector3::Type _tMax) const;

private:
	//------------------------------------------------------------------------------------------------------------------------------------
	SceneNode(TraceableObject* _object, int _depth);

	//------------------------------------------------------------------------------------------------------------------------------------
	void					Construct(TraceableObjects& _list, size_t _first, size_t _end);
	static void				Generate(const TraceableObjects& _list, size_t _first, size_t _end, AABB& _aabb);

	//------------------------------------------------------------------------------------------------------------------------------------
	static bool				AABBCompare(const TraceableObject* _a, const TraceableObject* _b, int _axis);
	static bool				AABBCompareX(const TraceableObject* _a, const TraceableObject* _b) { return AABBCompare(_a,_b,0); }
	static bool				AABBCompareY(const TraceableObject* _a, const TraceableObject* _b) { return AABBCompare(_a, _b, 1); }
	static bool				AABBCompareZ(const TraceableObject* _a, const TraceableObject* _b) { return AABBCompare(_a, _b, 2); }

private:
	//------------------------------------------------------------------------------------------------------------------------------------
	AABB					aabb;
	const TraceableObject*	object;
	SceneNode*				child[2];
	int						depth;
};
