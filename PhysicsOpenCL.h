#ifndef PHYSICS_OPENCL_H
#define PHYSICS_OPENCL_H

#include "ParticleSystem.h"

class PhysicsOpenCL {
public:
    PhysicsOpenCL();
    ~PhysicsOpenCL();
    
    bool initialize();
    void calculateForces(ParticleSystem& system, float k);
    
private:
    struct Impl;
    Impl* impl;
};

#endif