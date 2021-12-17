#include "config.h"
#include "render/Camera.h"

Camera::Camera()
{
	
}

Camera::Camera(float fov, float aspect, float n, float f) : fov(fov), aspect(aspect), n(n), f(f)
{
	pos = V4(0, 0, -10, 1);
	dir = V3(0, 1, 0);
	rotation = M4::identity();
	view = M4::identity();
	invView = M4::identity();
}

V3 Camera::getPos()
{
	return pos.toV3() * -1.f;
}

void Camera::DeriveTransforms()
{
	this->view = rotation * Translate(pos * -1.f);
	this->invView = Inverse(this->view);
}

void Camera::setTranslation(V4 pos)
{
	this->pos = pos;
	DeriveTransforms();
}

void Camera::setRot(V3 dir, float rad)
{
	rotation = Rotation(dir, rad);
	DeriveTransforms();
}

void Camera::setRot(M4 rotMat)
{
	rotation = rotMat;
}

M4 Camera::pv()
{
	return Transpose(projection(fov, aspect, n, f)) * this->view;
}