#pragma once

using namespace DirectX::SimpleMath;

//----------------------------------------------------------------------------------------------------------------------------------------
class Camera
{
public:
	//------------------------------------------------------------------------------------------------------------------------------------
	Camera();
	~Camera();

	//------------------------------------------------------------------------------------------------------------------------------------
	void				LookAt(const Vector3& _worldPosition);
	void				SetFOV(float _fieldOfView)		{ fieldOfView = _fieldOfView; UpdateInternals(); }
	void				SetAspect(float _aspect)		{ aspect = _aspect; UpdateInternals(); }

	void				CalculateRay(float _tx, float _ty, Vector3& _origin, Vector3& _direction) const;

	float				GetNearPlane() const { return nearPlane; }

private:
	//------------------------------------------------------------------------------------------------------------------------------------
	Vector3				position;
	Vector3				direction;
	Vector3				up;
	Vector3				right;
	float				nearPlane;
	float				fieldOfView;
	float				aspect;
	float				nearPlaneWidth;
	float				nearPlaneHeight;

private:
	//------------------------------------------------------------------------------------------------------------------------------------
	void				UpdateTransform();
	void				UpdateInternals();
};

