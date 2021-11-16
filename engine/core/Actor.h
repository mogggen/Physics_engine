#pragma once
#include "core/MathLibrary.h"

struct Actor
{
    M4 transform;
    float velocity;
    float mass;
    Quat torque;

	Actor();
    Actor(M4 transform, float mass, float velocity, Quat torque);
    M4 ToRotationMatrix();
};