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
	void				SetFOV(double _fieldOfView)			{ fieldOfView = _fieldOfView; UpdateInternals(); }
	void				SetAspect(double _aspect)			{ aspect = _aspect; UpdateInternals(); }

	void				CalculateRay(double _tx, double _ty, Ray& _ray) const;

private:
	//------------------------------------------------------------------------------------------------------------------------------------
	Vector3				position;
	Vector3				direction;
	Vector3				up;
	Vector3				right;
	double				nearPlane;
	double				fieldOfView;
	double				aspect;
	double				nearPlaneWidth;
	double				nearPlaneHeight;

private:
	//------------------------------------------------------------------------------------------------------------------------------------
	void				UpdateTransform();
	void				UpdateInternals();
};

