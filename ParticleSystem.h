#ifndef PARTICLE_SYSTEM_H
#define PARTICLE_SYSTEM_H

#include <vector>
#include <deque>
#include "Particle.h"

class ParticleSystem {
public:
    ParticleSystem();
    void addParticle(const Particle& particle);
    void update(float dt);
    void clearAccelerations();

    const std::vector<Particle>& getParticles() const;
    std::vector<Particle>& getParticles();

    // Manejo de trayectorias (simplificado)
    void startTracking(int index);
    void stopTracking();
    bool isTracking() const { return trackingIndex != -1; }
    int getTrackingIndex() const { return trackingIndex; }
    const std::deque<std::pair<float, float>>& getTrail() const { return trajectory; }

private:
    std::vector<Particle> particles;

    int trackingIndex = -1;
    std::deque<std::pair<float, float>> trajectory;
    static constexpr size_t MAX_TRAIL_LENGTH = 500;
};

#endif