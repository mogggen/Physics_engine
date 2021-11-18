#include "render/Actor.h"

Actor::Actor()
{
    transform = M4();
    mass = 1000.f;
    velocity = 0.f;
}

Actor::Actor(float mass, float velocity) : mass(mass), velocity(velocity)
{

}

M4 Actor::ToRotationMatrix()
{
    return M4();
}
