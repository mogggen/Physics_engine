#pragma once
#include "core/MathLibrary.h"
#include "ShaderResource.h"
#include <memory>

class Lightning
{
    vec3 pos;
    vec3 rgb;
    float intensity;
public:
    Lightning();
    Lightning(vec3 _pos, vec3 _rgb, float _intensity);
    ~Lightning();

    void setPos(vec3 _pos);
    vec3 getPos();

    void setColor(vec3 _rgb);
    vec3 getColor();

    void setIntensity(float _intensity);
    float getIntensity();

    void setFragShaderUniformVariables(const std::shared_ptr<ShaderResource>& shader, const vec3& cameraPos);
};
