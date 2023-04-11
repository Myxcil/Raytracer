#include "pch.h"
#include "Camera.h"

//---------------------------------------------------------------------------------------------------------------------------------------
Camera::Camera() :
	position(0, 0, -3),
	direction(0, 0, 1),
	nearPlane(1.0),
	fieldOfView(25.0),
	aspect(1.0),
	nearPlaneWidth(1.0),
	nearPlaneHeight(1.0)
{
	UpdateTransform();
	UpdateInternals();
}

//----------------------------------------------------------------------------------------------------------------------------------------
Camera::~Camera()
{
}

//----------------------------------------------------------------------------------------------------------------------------------------
void Camera::LookAt(const Vector3& _worldPosition)
{
	direction = _worldPosition - position;
	direction.Normalize();
	UpdateTransform();
}

//----------------------------------------------------------------------------------------------------------------------------------------
void Camera::CalculateRay(Vector3::Type _tx, Vector3::Type _ty, Ray& _ray) const
{
	_tx -= 0.5;
	_ty -= 0.5;

	Vector3 local = Vector3(nearPlaneWidth * _tx, nearPlaneHeight * _ty, nearPlane);

	_ray.origin = position;
	_ray.origin += right * local.x;
	_ray.origin += up * local.y;
	_ray.origin += direction * local.z;

	_ray.direction = _ray.origin - position;
	_ray.direction.Normalize();
}

//----------------------------------------------------------------------------------------------------------------------------------------
void Camera::UpdateTransform()
{
	up = Vector3(0, 1, 0);
	right = Vector3::Cross(up, direction);
	right.Normalize();
	up = Vector3::Cross(direction, right);
}

//----------------------------------------------------------------------------------------------------------------------------------------
void Camera::UpdateInternals()
{
	nearPlaneHeight = 2.0f * nearPlane * tan(M_PI * fieldOfView / 180.0);
	nearPlaneWidth = nearPlaneHeight * aspect;
}
