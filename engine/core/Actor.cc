#include "core/Actor.h"

Actor::Actor()
{
    transform = M4();
    mass = 1000.f;
    velocity = 0.f;
    torque = Quat();
}

Actor::Actor(M4 transform, float mass, float velocity, Quat torque) : transform(transform), mass(mass), velocity(velocity), torque(torque)
{

}
