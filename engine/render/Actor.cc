#include "config.h"
#include "render/Actor.h"

Actor::Actor()
{
    rotation = Translate(V4());
    orientation = Quaternion();
    angularVelocity = Quaternion();
    transform = rotation;
    linearVelocity = V4();
    mass = 0.f;
    angle = 0.f;
    angleVel = 0.f;
    isDynamic = false;
}



Actor::Actor(V3 _position,
    float _mass,
    V3 _linearVelocity,
    Quaternion _orientation,
    Quaternion _angularVelocity) :
    transform(Translate(V4(_position, 1))),
    mass(_mass),
    linearVelocity(_linearVelocity),
    orientation(_orientation),
    angularVelocity(_angularVelocity)
{
    
}

void Actor::apply_force(const V3& force, const float& dt)
{
    assert(mass, ("Actor %p has no mass", &this));
    const V3 acceleration = force * (1 / mass);
    linearVelocity = linearVelocity + acceleration * dt;
}

void Actor::apply_force(const V3& direction, const float& magF, const float& dt)
{
    assert(mass, ("Actor %p has no mass", &this));
    const V3 acceleration = direction * magF * (1 / mass);
    linearVelocity = linearVelocity + acceleration * dt;
}

// this function is cursed
void Actor::apply_linear_impulse(const Ray& ray, const V3& center_of_mass, const V3& contactPoint, const float& elasticity)
{
    assert(mass, ("Actor %p has no mass", &this));
    // how much should be linear velocity (the rest is angular velocity
    const float fraction = Dot(Normalize(transform.toV3() - ray.origin), Normalize(center_of_mass - ray.origin));
    float linear_impulse = 5.f;
    linearVelocity = linearVelocity + Normalize(ray.dir) * linear_impulse * (1 / mass);
}

void Actor::apply_angular_impulse(const Quaternion& impulse, const V4& contactPoint, const float& elasticity=.5f)
{
    // Calculate change in velocity due to the impulse
    Quaternion linearImpulse = impulse.normalized() * (1.0 + elasticity);

    // Update linear velocity
    linearVelocity = linearVelocity;// TODO + linearImpulse * (1 / mass);

    //// Calculate change in angular velocity due to the impulse


    //V4 r = contactPoint - position;
    //Quaternion angularImpulse = impulse.normalized() * r;

    //// Update angular velocity
    //angularVelocity = angularVelocity + angularImpulse// * inertiaTensorInverse
    //    ;
}

void Actor::update(const float& dt)
{
    //transform = (Translate(linearVelocity * dt) * transform);
    
    transform = 
    (Translate(linearVelocity * dt) *
    transform) *
    rotation
        ;
    //Quaternion deltaOrientation = Quaternion(1.0, angularVelocity.getX() * dt * 0.5,
    //                                                angularVelocity.getY() * dt * 0.5,
    //                                                angularVelocity.getZ() * dt * 0.5) * orientation;
    //orientation = orientation + deltaOrientation * 0.5;
    //orientation = orientation.normalized();
}


V3 calculateCollisionImpulse(const V3& relativeVelocity,
                                  const V3& contactNormal,
                                  float restitutionCoefficient,
                                  float totalInverseMass) {
    V3 f = relativeVelocity;
    V3 u = contactNormal;
    // Calculate the relative velocity along the contact normal
    float relativeVelocityAlongNormal = Dot(f, u);

    // If the relative velocity is separating (objects moving apart), no impulse is needed
    if (relativeVelocityAlongNormal > 0.0) {
        return V3();
    }

    // Calculate the impulse scalar (change in momentum)
    float impulseScalar = -(1.0 + restitutionCoefficient) * relativeVelocityAlongNormal;
    impulseScalar /= totalInverseMass;

    // Calculate the impulse vector (collision response)
    V3 impulse = contactNormal * impulseScalar;

    return impulse;
}
