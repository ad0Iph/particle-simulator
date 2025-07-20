#include "PhysicsSequential.h"
#include <cmath>

void PhysicsSequential::calculateForces(ParticleSystem& system, float k) {
    auto& particles = system.getParticles();
    const size_t n = particles.size();
    
    for (size_t i = 0; i < n; ++i) {
        for (size_t j = i + 1; j < n; ++j) {
            Particle& p1 = particles[i];
            Particle& p2 = particles[j];
            
            float dx = p2.position[0] - p1.position[0];
            float dy = p2.position[1] - p1.position[1];
            float distanceSq = dx*dx + dy*dy;
            
            // Evitar división por cero
            if (distanceSq < 1e-10f) continue;
            
            float distance = std::sqrt(distanceSq);
            // Cambia el exponente aquí:
            float force = -k * p1.charge * p2.charge / std::pow(distance, 1.0f);
            
            // Dirección de la fuerza
            float fx = force * dx / distance;
            float fy = force * dy / distance;
            
            // Aplicar fuerzas (Newton 3ra ley)
            p1.acceleration[0] += fx / p1.mass;
            p1.acceleration[1] += fy / p1.mass;
            p2.acceleration[0] -= fx / p2.mass;
            p2.acceleration[1] -= fy / p2.mass;
        }
    }
}