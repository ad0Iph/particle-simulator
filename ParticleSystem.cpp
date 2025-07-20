#include "ParticleSystem.h"

ParticleSystem::ParticleSystem() {}

void ParticleSystem::addParticle(const Particle& particle) {
    particles.push_back(particle);
}

void ParticleSystem::update(float dt) {
    for (auto& p : particles) {
        for (int i = 0; i < 3; ++i) {
            p.velocity[i] += p.acceleration[i] * dt;
            p.position[i] += p.velocity[i] * dt;
        }
    }

    // Actualizar trayectoria si estamos rastreando
    if (trackingIndex != -1 && trackingIndex < particles.size()) {
        const auto& p = particles[trackingIndex];
        trajectory.push_back({p.position[0], p.position[1]});
        
        // Mantener tamaño máximo
        if (trajectory.size() > MAX_TRAIL_LENGTH) {
            trajectory.pop_front();
        }
    }
}

void ParticleSystem::clearAccelerations() {
    for (auto& p : particles) {
        for (int i = 0; i < 3; ++i) {
            p.acceleration[i] = 0.0f;
        }
    }
}


const std::vector<Particle>& ParticleSystem::getParticles() const {
    return particles;
}

std::vector<Particle>& ParticleSystem::getParticles() {
    return particles;
}

void ParticleSystem::startTracking(int index) {
    if (index >= 0 && index < particles.size()) {
        trackingIndex = index;
        trajectory.clear();
    }
}

void ParticleSystem::stopTracking() {
    trackingIndex = -1;
    trajectory.clear();
}