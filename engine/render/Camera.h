#pragma once
#include "core/MathLibrary.h"

class Camera
{
	float fov, aspect, n, f;
	V4 pos, up;
	V3 dir;
	float rad = 0.f;
	M4 rotation;
public:
	Camera();
	Camera(float fov, float ratio, float n, float f);
	V3 getPos();
	void setPos(V4 pos);
	void setRot(V3 dir, float theta);
	void setRot(M4 rotMat);
	M4 pv();
};