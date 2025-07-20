#include "ParticleSystem.h"
#include "PhysicsSequential.h"
#include "PhysicsOpenCL.h"
#include "util.h" 

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <GLFW/glfw3.h>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <iostream>
#include <cmath>

const float WORLD_LEFT = -2.0f;
const float WORLD_RIGHT = 2.0f;
const float WORLD_BOTTOM = -2.0f;
const float WORLD_TOP = 2.0f;
int MAX_TRAJECTORY_HISTORY = 200;

int main() {
    if (!glfwInit()) return -1;
    GLFWwindow* window = glfwCreateWindow(900, 900, "CPU vs GPU particle simulator benchmark", NULL, NULL);
    if (!window) { glfwTerminate(); return -1; }
    glfwMakeContextCurrent(window);

    int NUM_PARTICLES = 2000;
    float k = 0.1f;
    float radius = 0.1f;
    static int new_num_particles = NUM_PARTICLES;
    static int selectedParticleIndex = -1;
    static float positive_ratio = 0.5f;
    static bool gKeyPressedLastFrame = false;
    static bool useGPU = false;

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigWindowsMoveFromTitleBarOnly = true;

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init();

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(WORLD_LEFT, WORLD_RIGHT, WORLD_BOTTOM, WORLD_TOP, -1, 1);
    glMatrixMode(GL_MODELVIEW);

    std::srand(static_cast<unsigned>(std::time(nullptr)));

    float dt = 0.01f;

    PhysicsOpenCL physicsGPU;
    if (!physicsGPU.initialize()) {
        std::cerr << "Error initializing openCL.\n";
        return -1;
    }
    ParticleSystem system;
    
    for (int i = 0; i < NUM_PARTICLES; ++i) {
        Particle p;
        p.position[0] = WORLD_LEFT + static_cast<float>(std::rand()) / RAND_MAX * (WORLD_RIGHT - WORLD_LEFT);
        p.position[1] = WORLD_BOTTOM + static_cast<float>(std::rand()) / RAND_MAX * (WORLD_TOP - WORLD_BOTTOM);
        p.velocity[0] = (static_cast<float>(std::rand()) / RAND_MAX - 0.5f) * 1.0f;
        p.velocity[1] = (static_cast<float>(std::rand()) / RAND_MAX - 0.5f) * 1.0f;
        p.acceleration[0] = 0.0f;
        p.acceleration[1] = 0.0f;
        p.mass = 1.0f;
        
        float r = static_cast<float>(std::rand()) / RAND_MAX;
        float charge_sign = (r < positive_ratio) ? 1.0f : -1.0f;
        p.charge = charge_sign;
        
        system.addParticle(p);
    }
    
    while (!glfwWindowShouldClose(window)) {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // --- Panel de Parámetros ---
        ImGui::SetNextWindowPos(ImVec2(0, 18));
        auto flags = ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove;
        ImGui::Begin("Parameters", nullptr, flags);

        ImGui::SliderFloat("Charge magnitude", &k, 0.0f, 1.0f, "%.3f");
        ImGui::SliderFloat("Radius", &radius, 0.001f, 0.05f, "%.3f");
        ImGui::SliderInt("Number of particles", &new_num_particles, 2, 6000);
        ImGui::SliderFloat("Positive charge ratio", &positive_ratio, 0.0f, 1.0f, "%.2f");
        ImGui::Checkbox("Use GPU", &useGPU);

        static float last_positive_ratio = positive_ratio;

        bool should_reinitialize =
            (new_num_particles != NUM_PARTICLES) ||
            (positive_ratio != last_positive_ratio);

        if (should_reinitialize) {
            NUM_PARTICLES = new_num_particles;
            last_positive_ratio = positive_ratio;

            system = ParticleSystem();
            for (int i = 0; i < NUM_PARTICLES; ++i) {
                Particle p;
                p.position[0] = WORLD_LEFT + static_cast<float>(std::rand()) / RAND_MAX * (WORLD_RIGHT - WORLD_LEFT);
                p.position[1] = WORLD_BOTTOM + static_cast<float>(std::rand()) / RAND_MAX * (WORLD_TOP - WORLD_BOTTOM);
                p.velocity[0] = (static_cast<float>(std::rand()) / RAND_MAX - 0.5f) * 1.0f;
                p.velocity[1] = (static_cast<float>(std::rand()) / RAND_MAX - 0.5f) * 1.0f;
                p.acceleration[0] = 0.0f;
                p.acceleration[1] = 0.0f;
                p.mass = 1.0f;
                
                float r = static_cast<float>(std::rand()) / RAND_MAX;
                float charge_sign = (r < positive_ratio) ? 1.0f : -1.0f;
                p.charge = charge_sign;
                
                system.addParticle(p);
            }
        }

        ImGui::End();

        // --- Panel de Estadísticas ---
        ImGui::SetNextWindowPos(ImVec2(740, 0));
        ImGui::Begin("Performance stats", nullptr, flags);
        ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
        ImGui::Text("Frame Time: %.3f ms", 1000.0f / ImGui::GetIO().Framerate);
        ImGui::End();

        system.clearAccelerations();
        if (!useGPU){
            PhysicsSequential::calculateForces(system, k);
        }
        else{
            physicsGPU.calculateForces(system, k);
        }
        system.update(dt);
        bounceParticles(system, WORLD_LEFT, WORLD_RIGHT, WORLD_BOTTOM, WORLD_TOP);
        
        bool gKeyPressed = (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS);
        if (gKeyPressed && !gKeyPressedLastFrame && !system.getParticles().empty()) {
            int randomIndex = std::rand() % system.getParticles().size();
            system.startTracking(randomIndex);
            selectedParticleIndex = randomIndex;
        }
        gKeyPressedLastFrame = gKeyPressed;

        if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
            system.stopTracking();
            selectedParticleIndex = -1;
        }
        
        renderParticles(system, radius);
        
        if (system.isTracking()) {
            renderTrail(system);
        }

        if (system.isTracking() && system.getTrackingIndex() < system.getParticles().size()) {
            const Particle& p = system.getParticles()[system.getTrackingIndex()];
            
            const float vectorScale = 0.2f; 
            
            float velLength = std::sqrt(p.velocity[0]*p.velocity[0] + p.velocity[1]*p.velocity[1]);
            if (velLength > 0.001f) {
                float velX = p.velocity[0] / velLength * vectorScale;
                float velY = p.velocity[1] / velLength * vectorScale;
                drawVector(p.position[0], p.position[1], velX, velY, 0.0f, 1.0f, 0.0f);
            }
            
            float accLength = std::sqrt(p.acceleration[0]*p.acceleration[0] + p.acceleration[1]*p.acceleration[1]);
            if (accLength > 0.001f) {
                float accX = p.acceleration[0] / accLength * vectorScale;
                float accY = p.acceleration[1] / accLength * vectorScale;
                drawVector(p.position[0], p.position[1], accX, accY, 1.0f, 0.0f, 0.0f);
            }
        }

        if (system.isTracking() && system.getTrackingIndex() < system.getParticles().size()) {
            const Particle& p = system.getParticles()[system.getTrackingIndex()];
            
            ImGui::SetNextWindowPos(ImVec2(0, 300), ImGuiCond_FirstUseEver);
            ImGui::SetNextWindowSize(ImVec2(250, 120), ImGuiCond_FirstUseEver);
            ImGui::Begin("Particle Info", nullptr, flags);
            
            ImGui::Text("Selected Particle: %d", system.getTrackingIndex());
            ImGui::Separator();
            ImGui::Text("Position:");
            ImGui::Text("  X: %.4f, Y: %.4f", p.position[0], p.position[1]);
            ImGui::Text("Velocity:");
            ImGui::Text("  X: %.4f, Y: %.4f", p.velocity[0], p.velocity[1]);
            ImGui::Text("Acceleration:");
            ImGui::Text("  X: %.4f, Y: %.4f", p.acceleration[0], p.acceleration[1]);
            
            ImGui::End();
        }

        // Renderizar la GUI
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}