#ifndef UTIL_H
#define UTIL_H
#include <GLFW/glfw3.h>
#include <cmath>
#include "ParticleSystem.h"

void drawVector(float startX, float startY, float dx, float dy, float r, float g, float b);
void drawCircle(float x, float y, float radius, float r, float g, float b);
void renderParticles(const ParticleSystem& system, float radius);
void bounceParticles(ParticleSystem& system, float leftLimit, float rightLimit, float bottomLimit, float topLimit);
void renderTrail(const ParticleSystem& system);

#endif 