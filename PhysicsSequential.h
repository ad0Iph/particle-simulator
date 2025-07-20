#ifndef PHYSICS_SEQUENTIAL_H
#define PHYSICS_SEQUENTIAL_H

#include "ParticleSystem.h"

class PhysicsSequential {
public:
    static void calculateForces(ParticleSystem& system, float k);
};

#endif