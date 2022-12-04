#include "config.h"
#include "render/Camera.h"

Camera::Camera()
{

}

Camera::Camera(float fov, float aspect, float n, float f) : fov(fov), aspect(aspect), n(n), f(f)
{
	pos = vec4(0, 0, 0);
	dir = vec4(0, 1, 0);
}

vec3 Camera::getPos()
{
	return pos.toV3();
}

void Camera::setPos(vec4 pos)
{
	this->pos = pos;
}

void Camera::setRot(vec4 dir, float rad)
{
	this->dir = dir;
	this->rad = rad;
}

mat4 Camera::pv()
{
	return projection(fov, aspect, n, f) * Rotation(dir, rad) * Translate(pos);
}