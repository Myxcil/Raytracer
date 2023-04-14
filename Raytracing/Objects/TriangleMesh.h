#pragma once

#include "TraceableObject.h"

//----------------------------------------------------------------------------------------------------------------------------------------
class TriangleMesh : public TraceableObject
{
public:
	//------------------------------------------------------------------------------------------------------------------------------------
	TriangleMesh(const Vector3& _center, const TCHAR* _filename, double _scale, Material* _material);

	//------------------------------------------------------------------------------------------------------------------------------------
	void			Raycast(HitInfo& _hitInfo, const Ray& _ray, double _tMin, double _tMax) const override;

private:
	//------------------------------------------------------------------------------------------------------------------------------------
	struct Vertex
	{
		Vector3			pos;
		Vector3			nrm;
		double	u,v;
	};

	//------------------------------------------------------------------------------------------------------------------------------------
	struct Triangle
	{
		int				v[3];
		Vector3			e1;
		Vector3			e2;
		double	lenSq1;
		double	lenSq2;
		double	dot12;
	};

private:
	//------------------------------------------------------------------------------------------------------------------------------------
	void			LoadMeshFromFile(const TCHAR* _filename, std::vector<Vector3>& _positions, std::vector<int>& _indices);
	void			BuildMesh(const std::vector<Vector3>& _positions, const std::vector<int>& _indices, double _scale);

	bool			IntersectTri(HitInfo& _hitInfo, const Ray& _ray, const Triangle& _tri, double _tMin, double _tMax) const;
	void			GetTriBarycentric(const Triangle& _tri, const Vector3& _p, Vector3& _result) const;
	Vector3			CalculateNormal(const Triangle& _tri, const Vector3 &_weight) const;

private:
	//------------------------------------------------------------------------------------------------------------------------------------
	std::vector<Vertex> vertices;
	std::vector<Triangle> triangles;
};