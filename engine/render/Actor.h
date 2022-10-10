#pragma once
#include "core/MathLibrary.h"

struct Actor
{ 
    vec4 position;
    float mass;
    float velocity;
    Quat torque;

	Actor();
    Actor(vec4 position, float mass, float velocity, Quat torque);
    mat4 ToRotationMatrix();
};