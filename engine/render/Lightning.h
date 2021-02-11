#pragma once
#include "core/MathLibrary.h"
#include "render/ShaderObject.h"
#include <vector>
#include <memory>

class Lighting
{
	V3 pos;
	V3 color;
	float intensity;

public:
	Lighting(V3 pos, V3 color, float intensity);
	~Lighting();
	void setIntensity(float intensityIn);
	void Lighting::setPos(V3 pos);
	void Lighting::setIntensity(V3 color);
	void bindLight(std::shared_ptr<ShaderObject> shaderObject, V3 cameraPos);
};
