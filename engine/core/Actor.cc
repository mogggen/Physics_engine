#include "core/Actor.h"

Actor::Actor()
{
    transform = M4();
    mass = 1000.f;
    torque = 
}

Actor::Actor(M4 transform, float mass, Quat torque) : transform(transform), mass(mass), torque(torque)
{

}

