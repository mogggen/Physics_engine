#include "config.h"
#include "render/Actor.h"

Actor::Actor()
{
    position = vec4();
    mass = 1.f;
    velocity = 0.f;
}

Actor::Actor(vec4 position, float mass, float velocity, Quat torque) : position(position), mass(mass), velocity(velocity), torque(torque)
{

}

mat4 Actor::ToRotationMatrix()
{
    return mat4();
}
