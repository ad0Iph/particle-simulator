#include "PhysicsOpenCL.h"
#include <CL/cl2.hpp>
#include <fstream>
#include <sstream>
#include <iostream>

class PhysicsOpenCL::Impl {
public:
    cl::Context context;
    cl::CommandQueue queue;
    cl::Kernel kernel;
    cl::Program program;
    bool initialized = false;
};

PhysicsOpenCL::PhysicsOpenCL() : impl(new Impl()) {}
PhysicsOpenCL::~PhysicsOpenCL() { delete impl; }

bool PhysicsOpenCL::initialize() {
    try {
        std::vector<cl::Platform> platforms;
        cl::Platform::get(&platforms);

        if (platforms.empty()) return false;

        cl::Platform platform = platforms[0];
        std::vector<cl::Device> devices;
        platform.getDevices(CL_DEVICE_TYPE_GPU, &devices);

        if (devices.empty()) return false;

        impl->context = cl::Context(devices);
        impl->queue = cl::CommandQueue(impl->context, devices[0]);

        // Cargar kernel desde archivo
        std::ifstream kernelFile("kernel.cl");
        if (!kernelFile) {
            std::cerr << "No se pudo abrir kernel.cl\n";
            return false;
        }
        std::stringstream kernelStream;
        kernelStream << kernelFile.rdbuf();
        std::string kernelCode = kernelStream.str();

        cl::Program::Sources sources;
        sources.push_back({kernelCode.c_str(), kernelCode.length()});

        impl->program = cl::Program(impl->context, sources);
        impl->program.build(devices);

        impl->kernel = cl::Kernel(impl->program, "calculateForces");
        impl->initialized = true;
        return true;
    }
    catch (std::exception& e) {
        std::cerr << "Standard exception: " << e.what() << "\n";
        return false;
    }
}

void PhysicsOpenCL::calculateForces(ParticleSystem& system, float k) {
    if (!impl->initialized) return;

    auto& particles = system.getParticles();
    const size_t count = particles.size();
    if (count == 0) return;

    // Buffers OpenCL
    cl::Buffer positions(impl->context, CL_MEM_READ_WRITE, sizeof(float) * 2 * count);
    cl::Buffer accelerations(impl->context, CL_MEM_READ_WRITE, sizeof(float) * 2 * count);
    cl::Buffer charges(impl->context, CL_MEM_READ_ONLY, sizeof(float) * count);
    cl::Buffer masses(impl->context, CL_MEM_READ_ONLY, sizeof(float) * count);

    // Copiar datos al dispositivo
    std::vector<float> posData(2 * count);
    std::vector<float> chargeData(count);
    std::vector<float> massData(count);
    for (size_t i = 0; i < count; ++i) {
        posData[2*i] = particles[i].position[0];
        posData[2*i+1] = particles[i].position[1];
        chargeData[i] = particles[i].charge;
        massData[i] = particles[i].mass;
    }
    impl->queue.enqueueWriteBuffer(positions, CL_TRUE, 0, sizeof(float) * 2 * count, posData.data());
    impl->queue.enqueueWriteBuffer(charges, CL_TRUE, 0, sizeof(float) * count, chargeData.data());
    impl->queue.enqueueWriteBuffer(masses, CL_TRUE, 0, sizeof(float) * count, massData.data());

    // Configurar kernel
    impl->kernel.setArg(0, positions);
    impl->kernel.setArg(1, accelerations);
    impl->kernel.setArg(2, charges);
    impl->kernel.setArg(3, masses);
    impl->kernel.setArg(4, k);
    impl->kernel.setArg(5, static_cast<int>(count));

    // Ejecutar kernel
    impl->queue.enqueueNDRangeKernel(impl->kernel, cl::NullRange, cl::NDRange(count));

    // Leer resultados
    std::vector<float> accelData(2 * count);
    impl->queue.enqueueReadBuffer(accelerations, CL_TRUE, 0, sizeof(float) * 2 * count, accelData.data());

    // Actualizar aceleraciones en CPU
    for (size_t i = 0; i < count; ++i) {
        particles[i].acceleration[0] = accelData[2*i];
        particles[i].acceleration[1] = accelData[2*i+1];
    }
}