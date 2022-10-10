#pragma once
#include "core/MathLibrary.h"

class Camera
{
	float fov, aspect, n, f;
	vec4 pos, up;
	vec4 dir;
	float rad = 0.f;
public:
	Camera(float fov, float ratio, float n, float f);
	vec3 getPos();
	void setPos(vec4 pos);
	void setRot(vec4 dir, float theta);
	mat4 pv();
};