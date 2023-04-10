#include "pch.h"
#include "TriangleMesh.h"

//----------------------------------------------------------------------------------------------------------------------------------------
TriangleMesh::TriangleMesh(const Vector3& _center, const TCHAR* _filename, Material* _material) :
	TraceableObject(_center, _material)
{
	LoadMeshFromFile(_filename);
	RecalculateNormals();
	RecalculateAABB();
}

//----------------------------------------------------------------------------------------------------------------------------------------
void TriangleMesh::Raycast(HitInfo& _hitInfo, const Ray& _ray, Vector3::Type _tMin, Vector3::Type _tMax) const
{
	aabb.Raycast(_hitInfo, _ray, _tMin, _tMax);
	if (_hitInfo.isHit)
	{
		OnHit(_hitInfo);
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------
void TriangleMesh::LoadMeshFromFile(const TCHAR* _filename)
{
	FILE* file;
	_wfopen_s(&file, _filename, _T("r"));
	if (file == nullptr)
	{
		Helper::Log(_T("can't open file: %s"), _filename);
		return;
	}

	while(true)
	{
		char lineHeader[128] = { 0 };
		int res = fscanf_s(file, "%s", lineHeader, 128);
		if (res == EOF)
			break;

		if (strcmp(lineHeader, "v") == 0)
		{
			Vector3 v;
			fscanf_s(file, "%lf %lf %lf", &v.x, &v.y, &v.z);
			vertices.push_back(v);
		}
		if (strcmp(lineHeader, "f") == 0)
		{
			Triangle t;
			fscanf_s(file, "%d %d %d", &t.v[0],&t.v[1],&t.v[2]);
			t.v[0] -= 1;
			t.v[1] -= 1;
			t.v[2] -= 1;
			triangles.push_back(t);
		}
	}

	fclose(file);
}

//----------------------------------------------------------------------------------------------------------------------------------------
void TriangleMesh::RecalculateNormals()
{
	for (size_t i = 0; i < triangles.size(); ++i)
	{
		Triangle& tri = triangles[i];

		Vector3 v0 = vertices[tri.v[0]];
		Vector3 v1 = vertices[tri.v[1]];
		Vector3 v2 = vertices[tri.v[2]];

		Vector3 e0 = v1 - v0;
		Vector3 e1 = v2 - v0;

		Vector3 n = Vector3::Cross(e0,e1);
		n.Normalize();
		tri.normal = n;
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------
void TriangleMesh::RecalculateAABB()
{
	aabb = AABB(vertices[0],vertices[0]);
	for (size_t i = 0; i < vertices.size(); ++i)
	{
		aabb.Merge(vertices[i]);
	}
}
