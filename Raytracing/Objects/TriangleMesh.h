#pragma once

#include "TraceableObject.h"

//----------------------------------------------------------------------------------------------------------------------------------------
class TriangleMesh : public TraceableObject
{
public:
	//------------------------------------------------------------------------------------------------------------------------------------
	TriangleMesh(const Vector3& _center, const TCHAR* _filename, Material* _material);

	//------------------------------------------------------------------------------------------------------------------------------------
	void			Raycast(HitInfo& _hitInfo, const Ray& _ray, Vector3::Type _tMin, Vector3::Type _tMax) const override;

private:
	//------------------------------------------------------------------------------------------------------------------------------------
	struct Triangle
	{
		int			v[3];
		Vector3		normal;
	};

private:
	//------------------------------------------------------------------------------------------------------------------------------------
	void			LoadMeshFromFile(const TCHAR* _filename);
	void			RecalculateNormals();
	void			RecalculateAABB();

private:
	//------------------------------------------------------------------------------------------------------------------------------------
	std::vector<Vector3> vertices;
	std::vector<Triangle> triangles;
};