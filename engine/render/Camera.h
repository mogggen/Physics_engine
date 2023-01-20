#pragma once
#include "core/MathLibrary.h"

class Camera
{
	float fov, aspect, n, f;
	vec4 pos, up;
	mat4 rot;
public:
	Camera();
	Camera(float fov, float ratio, float n, float f);
	void setNearPlane(float n);
	void setFarPlane(float f);
	float getNearPlane();
	float getFarPlane();
	vec3 getPos();
	void setPos(vec4 pos);
	void addRot(vec4 dir, float theta);
	void addRot(mat4 mat);
	mat4 pv();
};