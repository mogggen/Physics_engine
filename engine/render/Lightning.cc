#include "config.h"
#include "Lightning.h"


Lightning::Lightning(vec3 _pos, vec3 _rgb, float _intensity)
{
    pos = _pos;
    rgb = _rgb;
    intensity = _intensity;
}

Lightning::~Lightning()
{

}


void Lightning::setPos(vec3 _pos)
{
    pos = _pos;
}

vec3 Lightning::getPos()
{
    return pos;
}

void Lightning::setColor(vec3 _rgb)
{
    rgb = _rgb;
}

vec3 Lightning::getColor()
{
    return rgb;
}

void Lightning::setIntensity(float _intensity)
{
    intensity = _intensity;
}

float Lightning::getIntensity()
{
    return intensity;
}

void Lightning::bindLight(std::shared_ptr<ShaderResource> shader, vec3 cameraPos)
{
    shader->setV3(rgb, "lightColor");
    shader->setV3(pos, "lightPos");
    shader->setFloat(1, "lightIntensity");

    shader->setV3(cameraPos, "camPos");
    shader->setFloat(1, "specularIntensity");
}
