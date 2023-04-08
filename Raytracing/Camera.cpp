#include "pch.h"
#include "Camera.h"

using namespace DirectX;

//---------------------------------------------------------------------------------------------------------------------------------------
Camera::Camera() :
	nearPlane(1.0f),
	fieldOfView(55.0f),
	aspect(1.0f),
	nearPlaneWidth(1.0f),
	nearPlaneHeight(1.0f)
{
	position = Vector3(1, 1.5f, -4);
	direction = Vector3(-0.5f, -0.1f, 1);
	direction.Normalize();

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
}

//----------------------------------------------------------------------------------------------------------------------------------------
void Camera::CalculateRay(float _tx, float _ty, Vector3& _origin, Vector3& _direction) const
{
	_tx -= 0.5f;
	_ty -= 0.5f;

	Vector3 local = Vector3(nearPlaneWidth * _tx, nearPlaneHeight * _ty, nearPlane);
	
	_origin = position;
	_origin += right * local.x;
	_origin += up * local.y;
	_origin += direction * local.z;

	_direction = _origin - position;
	_direction.Normalize();
}

//----------------------------------------------------------------------------------------------------------------------------------------
void Camera::UpdateTransform()
{
	up = Vector3(0,1,0);
	right = up.Cross(direction);
	right.Normalize();
	up = direction.Cross(right);
}

//----------------------------------------------------------------------------------------------------------------------------------------
void Camera::UpdateInternals()
{
	nearPlaneHeight = 2.0f * nearPlane * tanf(0.5f * 3.1415296f * fieldOfView / 180.0f);
	nearPlaneWidth = nearPlaneHeight * aspect;
}
