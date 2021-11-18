#pragma once
#include "core/MathLibrary.h"

struct Actor
{ 
    M4 transform;
    float mass;
    float velocity;
    Quat torque;

	Actor();
    Actor(float mass, float velocity);
    M4 ToRotationMatrix();
};