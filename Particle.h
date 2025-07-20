#ifndef PARTICLE_H
#define PARTICLE_H
#include <vector>

struct Particle {
    float position[3];     // x, y, z
    float velocity[3];     // vx, vy, vz
    float acceleration[3]; // ax, ay, az
    float mass;
    float charge;
};

#endif