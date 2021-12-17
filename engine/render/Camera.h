#pragma once
#include "core/MathLibrary.h"

class Camera
{
	float fov, aspect, n, f;
	V4 pos;
	V4 up;
	V3 dir;
	float rad = 0.f;
	M4 rotation;
	M4 view;
	M4 invView;
	void DeriveTransforms();
public:
	Camera();
	Camera(float fov, float ratio, float n, float f);
	V3 getPos();
	void setTranslation(V4 pos);
	void setRot(V3 dir, float theta);
	void setRot(M4 rotMat);
	M4 pv();
};