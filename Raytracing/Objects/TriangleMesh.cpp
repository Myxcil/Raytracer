#include "pch.h"
#include "TriangleMesh.h"
#include "Sphere.h"

//----------------------------------------------------------------------------------------------------------------------------------------
TriangleMesh::TriangleMesh(const Vector3& _center, const TCHAR* _filename, Vector3::Type _scale, Material* _material) :
	TraceableObject(_center, _material)
{
	std::vector<Vector3> positions;
	std::vector<int> indices;
	
	if (_filename != nullptr)
	{
		LoadMeshFromFile(_filename, positions, indices);
	}
	else
	{
		positions.push_back(Vector3(-1,-1, 0));
		positions.push_back(Vector3( 1,-1, 0));
		positions.push_back(Vector3( 0, 1, 0));
		indices.push_back(1);
		indices.push_back(2);
		indices.push_back(3);
	}
	BuildMesh(positions, indices, _scale);
}

//----------------------------------------------------------------------------------------------------------------------------------------
void TriangleMesh::LoadMeshFromFile(const TCHAR* _filename, std::vector<Vector3>& _positions, std::vector<int>& _indices)
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
			_positions.push_back(v);
		}
		if (strcmp(lineHeader, "f") == 0)
		{
			int a,b,c;
			fscanf_s(file, "%d %d %d", &a, &b, &c);
			_indices.push_back(a);
			_indices.push_back(b);
			_indices.push_back(c);
		}
	}

	fclose(file);
}

//----------------------------------------------------------------------------------------------------------------------------------------
void TriangleMesh::BuildMesh(const std::vector<Vector3>& _positions, const std::vector<int>& _indices, Vector3::Type _scale)
{
	size_t triCount = _indices.size() / 3;
	assert(triCount * 3 == _indices.size());
	
	vertices.reserve(_positions.size());
	triangles.reserve(triCount);

	for (size_t i = 0; i < _positions.size(); ++i)
	{
		vertices.push_back({ center + _scale * _positions[i], Vector3(0,0,0), 0, 0 });
	}

	for (size_t i = 0; i < triCount; ++i)
	{
		Triangle tri;

		for (int j = 0; j < 3; ++j)
		{
			tri.v[j] = _indices[i * 3 + j] - 1;
		}

		Vector3 v0 = vertices[tri.v[0]].pos;

		tri.e1 = vertices[tri.v[1]].pos - v0;
		tri.e2 = vertices[tri.v[2]].pos - v0;
		tri.lenSq1 = tri.e1.LengthSq();
		tri.lenSq2 = tri.e2.LengthSq();
		tri.dot12 = Vector3::Dot(tri.e1, tri.e2);

		Vector3 planeNormal = Vector3::Cross(tri.e1, tri.e2);
		planeNormal.Normalize();
		for (int j = 0; j < 3; ++j)
		{
			vertices[tri.v[j]].nrm += planeNormal;
		}

		triangles.push_back(tri);
	}

	aabb = AABB(vertices[0].pos, vertices[0].pos);
	for (size_t i = 0; i < vertices.size(); ++i)
	{
		vertices[i].nrm.Normalize();
		aabb.Merge(vertices[i].pos);
	}
	aabb.EnsureMinSize(0.01);
}

//----------------------------------------------------------------------------------------------------------------------------------------
void TriangleMesh::Raycast(HitInfo& _hitInfo, const Ray& _ray, Vector3::Type _tMin, Vector3::Type _tMax) const
{
	if (!aabb.Hit(_ray, _tMin, _tMax))
		return;

	Vector3::Type nearest = _tMax;
	for (size_t i = 0; i < triangles.size(); ++i)
	{
		IntersectTri(_hitInfo, _ray, triangles[i], _tMin, nearest);
		nearest = _hitInfo.distance;
	}

	if (_hitInfo.isHit)
	{
		_hitInfo.point += center;
		OnHit(_hitInfo);
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------
bool TriangleMesh::IntersectTri(HitInfo& _hitInfo, const Ray& _ray, const Triangle& _tri, Vector3::Type _tMin, Vector3::Type _tMax) const
{
	Vector3 pvec = Vector3::Cross(_ray.direction, _tri.e2);
	Vector3::Type det = Vector3::Dot(_tri.e1, pvec);
	if (det != 0)
	{
		Vector3::Type invDet = 1.0 / det;
		Vector3 tvec = _ray.origin - vertices[_tri.v[0]].pos;

		Vector3::Type u = Vector3::Dot(tvec, pvec) * invDet;
		if (u >= 0 && u <= 1.0)
		{
			Vector3 qvec = Vector3::Cross(tvec, _tri.e1);
			Vector3::Type v = Vector3::Dot(_ray.direction, qvec) * invDet;
			if (v >= 0 && (u + v) <= 1.0)
			{
				Vector3::Type temp = Vector3::Dot(_tri.e2, qvec) * invDet;
				if (temp >= _tMin && temp <= _tMax)
				{
					_hitInfo.isHit = true;
					_hitInfo.distance = temp;
					_hitInfo.point = _ray.GetPoint(temp);

					GetTriBarycentric(_tri, _hitInfo.point, _hitInfo.uvw);

					_hitInfo.SetNormal(_ray.direction, CalculateNormal(_tri, _hitInfo.uvw));

					return true;
				}
			}
		}
	}
	return false;
}

//----------------------------------------------------------------------------------------------------------------------------------------
void TriangleMesh::GetTriBarycentric(const Triangle& _tri, const Vector3& _p, Vector3& _result) const
{
	Vector3 q = _p - vertices[_tri.v[0]].pos;

	Vector3::Type d00 = _tri.lenSq1;
	Vector3::Type d01 = _tri.dot12;
	Vector3::Type d11 = _tri.lenSq2;
	Vector3::Type d20 = Vector3::Dot(q, _tri.e1);
	Vector3::Type d21 = Vector3::Dot(q, _tri.e2);

	Vector3::Type d = d00 * d11 - d01 * d01;
	Vector3::Type v = (d11 * d20 - d01 * d21) / d;
	Vector3::Type w = (d00 * d21 - d01 * d20) / d;
	Vector3::Type u = 1.0 - v - w;

	_result = Vector3(u,v,w);
}

//----------------------------------------------------------------------------------------------------------------------------------------
Vector3 TriangleMesh::CalculateNormal(const Triangle& _tri, const Vector3& _weight) const
{
	Vector3 n;
	for (int i = 0; i < 3; ++i)
	{
		n += _weight.v[i] * vertices[_tri.v[i]].nrm;
	}
	n.Normalize();
	return n;
}