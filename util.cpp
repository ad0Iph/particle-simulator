#include "util.h"

void drawVector(float startX, float startY, float dx, float dy, float r, float g, float b) {
    glColor3f(r, g, b);
    
    // Dibujar línea principal
    glBegin(GL_LINES);
    glVertex2f(startX, startY);
    glVertex2f(startX + dx, startY + dy);
    glEnd();
    
    // Dibujar punta de flecha
    if (std::abs(dx) > 0.001f || std::abs(dy) > 0.001f) {
        float angle = atan2(dy, dx);
        float arrowSize = 0.05f;
        
        glBegin(GL_TRIANGLES);
        glVertex2f(startX + dx, startY + dy);
        glVertex2f(startX + dx - arrowSize * cos(angle - 0.3f), 
                   startY + dy - arrowSize * sin(angle - 0.3f));
        glVertex2f(startX + dx - arrowSize * cos(angle + 0.3f), 
                   startY + dy - arrowSize * sin(angle + 0.3f));
        glEnd();
    }
}

void drawCircle(float x, float y, float radius, float r, float g, float b) {
    glColor3f(r, g, b);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(x, y);
    int num_segments = 18;
    for (int i = 0; i <= num_segments; ++i) {
        float angle = 2.0f * 3.1415926f * float(i) / float(num_segments);
        float dx = radius * cosf(angle);
        float dy = radius * sinf(angle);
        glVertex2f(x + dx, y + dy);
    }
    glEnd();
}

void renderParticles(const ParticleSystem& system, float radius) {
    glClear(GL_COLOR_BUFFER_BIT);
    for (const auto& p : system.getParticles()) {
        if (p.charge > 0)
            drawCircle(p.position[0], p.position[1], radius, 1.0f, 0.5f, 0.0f); // rojo
        else
            drawCircle(p.position[0], p.position[1], radius, 0.5f, 0.0f, 0.5f); // azul
    }
}

void bounceParticles(ParticleSystem& system, float leftLimit, float rightLimit, float bottomLimit, float topLimit) {
    for (auto& p : system.getParticles()) {
        if (p.position[0] < leftLimit) {
            p.position[0] = leftLimit;
            p.velocity[0] *= -1;
        }
        if (p.position[0] > rightLimit) {
            p.position[0] = rightLimit;
            p.velocity[0] *= -1;
        }
        if (p.position[1] < bottomLimit) {
            p.position[1] = bottomLimit;
            p.velocity[1] *= -1;
        }
        if (p.position[1] > topLimit) {
            p.position[1] = topLimit;
            p.velocity[1] *= -1;
        }
    }
}

void renderTrail(const ParticleSystem& system) {
    if (!system.isTracking()) return;
    
    const auto& trajectory = system.getTrail();
    if (trajectory.size() < 2) return;
    
    glColor3f(1.0f, 1.0f, 1.0f); // Color blanco
    
    // Dibujar línea punteada
    glEnable(GL_LINE_STIPPLE);
    glLineStipple(1, 0x00FF); // Patrón de puntos: 1 encendido, 1 apagado
    glBegin(GL_LINE_STRIP);
    for (const auto& point : trajectory) {
        glVertex2f(point.first, point.second);
    }
    glEnd();
    glDisable(GL_LINE_STIPPLE);
}