cmake_minimum_required(VERSION 3.10)
project(ParticleSystemApp)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenCL REQUIRED)
find_package(OpenGL REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_search_module(GLFW REQUIRED glfw3)

set(IMGUI_DIR ${CMAKE_SOURCE_DIR}/lib/imgui)

set(IMGUI_SOURCES
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
    ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
)

set(IMGUI_HEADERS
    ${IMGUI_DIR}/imgui.h
    ${IMGUI_DIR}/imconfig.h
    ${IMGUI_DIR}/imgui_internal.h
    ${IMGUI_DIR}/backends/imgui_impl_glfw.h
    ${IMGUI_DIR}/backends/imgui_impl_opengl3.h
)

add_library(imgui STATIC ${IMGUI_SOURCES} ${IMGUI_HEADERS})
target_include_directories(imgui PUBLIC
    ${IMGUI_DIR}
    ${IMGUI_DIR}/backends
)

set(SHARED_SOURCES
    ParticleSystem.cpp
    PhysicsSequential.cpp
    PhysicsOpenCL.cpp
    Particle.cpp
    util.cpp
)

add_executable(ParticleSystem main.cpp ${SHARED_SOURCES})
target_link_libraries(ParticleSystem imgui glfw OpenGL::GL OpenCL::OpenCL)
target_include_directories(ParticleSystem PRIVATE ${GLFW_INCLUDE_DIRS})

configure_file(kernel.cl kernel.cl COPYONLY)
