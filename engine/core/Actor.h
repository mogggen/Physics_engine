#pragma once
#include "core/MathLibrary.h"

struct Actor
{
	Actor();
    Actor(M4 transform, float mass, Quat torque);
    M4 transform;
    float mass;
    Quat torque;
    M4 ToRotationMatrix();
};