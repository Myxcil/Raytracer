#pragma once

#include "TraceableObject.h"

//----------------------------------------------------------------------------------------------------------------------------------------
class TriangleMesh : public TraceableObject
{
public:
	//------------------------------------------------------------------------------------------------------------------------------------
	TriangleMesh(const Vector3& _center, const TCHAR* _filename, Vector3::Type _scale, Material* _material);

	//------------------------------------------------------------------------------------------------------------------------------------
	void			Raycast(HitInfo& _hitInfo, const Ray& _ray, Vector3::Type _tMin, Vector3::Type _tMax) const override;

private:
	//------------------------------------------------------------------------------------------------------------------------------------
	struct Vertex
	{
		Vector3			pos;
		Vector3			nrm;
		Vector3::Type	u,v;
	};

	//------------------------------------------------------------------------------------------------------------------------------------
	struct Triangle
	{
		int				v[3];
		Vector3			e1;
		Vector3			e2;
		Vector3::Type	lenSq1;
		Vector3::Type	lenSq2;
		Vector3::Type	dot12;
	};

private:
	//------------------------------------------------------------------------------------------------------------------------------------
	void			LoadMeshFromFile(const TCHAR* _filename, std::vector<Vector3>& _positions, std::vector<int>& _indices);
	void			BuildMesh(const std::vector<Vector3>& _positions, const std::vector<int>& _indices, Vector3::Type _scale);

	bool			IntersectTri(HitInfo& _hitInfo, const Ray& _ray, const Triangle& _tri, Vector3::Type _tMin, Vector3::Type _tMax) const;
	void			GetTriBarycentric(const Triangle& _tri, const Vector3& _p, Vector3& _result) const;
	Vector3			CalculateNormal(const Triangle& _tri, const Vector3 &_weight) const;

private:
	//------------------------------------------------------------------------------------------------------------------------------------
	std::vector<Vertex> vertices;
	std::vector<Triangle> triangles;
};