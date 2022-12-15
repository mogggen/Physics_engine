#include "config.h"
#include "render/Camera.h"

Camera::Camera()
{
	rot = mat4();
}

Camera::Camera(float fov, float aspect, float n, float f) : fov(fov), aspect(aspect), n(n), f(f)
{
	rot = Rotation(vec4(0, 1, 0), 0.f);
}

vec3 Camera::getPos()
{
	return pos.toV3();
}

void Camera::setPos(vec4 pos)
{
	this->pos = pos;
}

void Camera::addRot(vec4 dir, float rad)
{
	rot = rot * Rotation(dir, rad);
}

void Camera::addRot(mat4 mat)
{
	rot = rot * mat;
}

mat4 Camera::pv()
{
	return projection(fov, aspect, n, f) * rot * Translate(pos);
}