#pragma once
#include <memory>
#include "core/MathLibrary.h"

struct Actor
{
    float mass;
    float elasticity; // Coefficient of restitution
    Quaternion orientation;
    Quaternion angularVelocity;
    V4 position; // Quaternion representing the object's position in 3D space
    V4 linearVelocity;

    Actor();
    Actor(V3 _position, float _mass, V3 _linearVelocity,Quaternion _orientation, Quaternion _angularVelocity);
    
    void update(const float& dt);
    void apply_force(const V3& force, const float& dt);
    void apply_force(const V3& direction, const float& magF, const float& dt);
    void apply_Momentum();
    void apply_linear_impulse(const Ray & ray, const V3& center_of_mass, const V3& contact_point, const float& elasticity=.5f);
    void apply_angular_impulse(const Quaternion& contactImpuse, const V4& contactPoint, const float& elasticity);
    M4 get_world_space_transform();
};