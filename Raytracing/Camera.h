#pragma once

//----------------------------------------------------------------------------------------------------------------------------------------
class Camera
{
public:
	//------------------------------------------------------------------------------------------------------------------------------------
	Camera();
	~Camera();

	//------------------------------------------------------------------------------------------------------------------------------------
	void				SetPosition(const Vector3& _worldPosition)	{ position = _worldPosition; }
	void				LookAt(const Vector3& _worldPosition);
	void				SetFOV(Vector3::Type _fieldOfView)			{ fieldOfView = _fieldOfView; UpdateInternals(); }
	void				SetAspect(Vector3::Type _aspect)			{ aspect = _aspect; UpdateInternals(); }

	void				CalculateRay(Vector3::Type _tx, Vector3::Type _ty, Ray& _ray) const;

	Vector3::Type		GetNearPlane() const { return nearPlane; }

private:
	//------------------------------------------------------------------------------------------------------------------------------------
	Vector3				position;
	Vector3				direction;
	Vector3				up;
	Vector3				right;
	Vector3::Type		nearPlane;
	Vector3::Type		fieldOfView;
	Vector3::Type		aspect;
	Vector3::Type		nearPlaneWidth;
	Vector3::Type		nearPlaneHeight;

private:
	//------------------------------------------------------------------------------------------------------------------------------------
	void				UpdateTransform();
	void				UpdateInternals();
};

