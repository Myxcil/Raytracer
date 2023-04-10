#include "pch.h"
#include "SceneNode.h"

//----------------------------------------------------------------------------------------------------------------------------------------
SceneNode::SceneNode(TraceableObjects& _list, size_t _first, size_t _end, int _depth) : 
	object(NULL),
	depth(_depth)
{
	child[0] = nullptr;
	child[1] = nullptr;

	auto list = _list;
	Construct(list, _first, _end);
}

//----------------------------------------------------------------------------------------------------------------------------------------
SceneNode::SceneNode(TraceableObject* _object, int _depth) :
	object(_object),
	depth(_depth)
{
	child[0] = nullptr;
	child[1] = nullptr;
}

//----------------------------------------------------------------------------------------------------------------------------------------
SceneNode::~SceneNode()
{
}

//----------------------------------------------------------------------------------------------------------------------------------------
void SceneNode::Construct(TraceableObjects& _list, size_t _first, size_t _end)
{
	size_t count = _end - _first;
	if (count == 1)
	{
		object = _list[_first];
	}
	else
	{
		Generate(_list, _first, _end, aabb);
		Vector3 size = aabb.Size();
		size_t mid = _first + count/2;

		int axis = 0;
		auto compareFunc = AABBCompareX;
		if (size.y >= size.x && size.y >= size.z)
		{
			axis = 1;
			compareFunc = AABBCompareY;
		}
		else if (size.z >= size.x && size.z >= size.y)
		{
			axis = 2;
			compareFunc = AABBCompareZ;
		}

		if (count == 2)
		{
			if (compareFunc(_list[_first], _list[_first + 1]))
			{
				child[0] = new SceneNode(_list[_first], depth + 1);
				child[1] = new SceneNode(_list[_first + 1], depth + 1);
			}
			else
			{
				child[0] = new SceneNode(_list[_first + 1], depth + 1);
				child[1] = new SceneNode(_list[_first], depth + 1);
			}
		}
		else
		{
			std::sort(_list.begin() + _first, _list.begin() + _end, compareFunc);
			child[0] = new SceneNode(_list, _first, mid, depth + 1);
			child[1] = new SceneNode(_list, mid, _end, depth + 1);
		}
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------
void SceneNode::Generate(const TraceableObjects& _list, size_t _first, size_t _end, AABB& _aabb)
{
	for (size_t i = _first; i < _end; ++i)
	{
		assert(_list[i]->GetAABB().IsValid());
		if (!_aabb.IsValid())
		{
			_aabb = _list[i]->GetAABB();
		}
		else
		{
			_aabb.Merge(_list[i]->GetAABB());
		}
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------
bool SceneNode::AABBCompare(const TraceableObject* _a, const TraceableObject* _b, int _axis)
{
	const AABB& boxA = _a->GetAABB();
	const AABB& boxB = _b->GetAABB();

	const Vector3& minA = boxA.Min();
	const Vector3& minB = boxB.Min();

	return minA.v[_axis] < minB.v[_axis];
}

//----------------------------------------------------------------------------------------------------------------------------------------
void SceneNode::Raycast(HitInfo& _hitInfo, const Ray& _ray, Vector3::Type _tMin, Vector3::Type _tMax) const
{
	if (object != NULL)
	{
		object->Raycast(_hitInfo, _ray, _tMin, _tMax);
	}
	else
	{
		if (aabb.Hit(_ray, _tMin, _tMax))
		{
			if (child[0] != nullptr)
			{
				child[0]->Raycast(_hitInfo, _ray, _tMin, _tMax);
			}
			if (child[1] != nullptr && child)
			{
				child[1]->Raycast(_hitInfo, _ray, _tMin, _tMax);
			}
		}
	}
}