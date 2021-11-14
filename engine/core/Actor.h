#pragma once
#include "core/MathLibrary.h"

class Actor
{
    M4 transform;
    float mass;
    Quat torque;
public:
	Actor();
    Actor(M4 transform, float mass, Quat torque);
    M4 ToRotationMatrix();
};