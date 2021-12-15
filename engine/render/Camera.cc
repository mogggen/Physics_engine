#include "config.h"
#include "render/Camera.h"

Camera::Camera()
{
	
}

Camera::Camera(float fov, float aspect, float n, float f) : fov(fov), aspect(aspect), n(n), f(f)
{
	pos = V4(0, 0, 0);
	dir = V3(0, 1, 0);
}

V3 Camera::getPos()
{
	return pos.toV3();
}

void Camera::setPos(V4 pos)
{
	this->pos = pos;
}

void Camera::setRot(V3 dir, float rad)
{
	rotation = Rotation(dir, rad);
}

void Camera::setRot(M4 rotMat)
{
	rotation = rotMat;
}

M4 Camera::pv()
{
	return projection(fov, aspect, n, f) * rotation * Translate(pos);
}