#include "config.h"
#include "Lightning.h"
#include "ShaderObject.h"
#include <memory>
#include <vector>
#include "core/MathLibrary.h"

Lighting::Lighting(V3 pos, V3 color, float intensity) : pos(pos), color(color), intensity(intensity)
{
	
}

Lighting::~Lighting()
{

}

void Lighting::setIntensity(float intensity)
{
	this->intensity;
}

void Lighting::setPos(V3 pos)
{
	this->pos = pos;
}

void Lighting::setIntensity(V3 color)
{
	this->color = color;
}

// Sets all values required for lighting
void Lighting::bindLight(std::shared_ptr<ShaderObject> shaderObject, V3 cameraPos) {

	shaderObject->setV3(color, "lightColor");
	shaderObject->setV3(pos, "lightPos");
	shaderObject->setF(intensity, "intensity");

	shaderObject->setV3(cameraPos, "viewPos");
	shaderObject->setF(1, "specIntensity");
}
