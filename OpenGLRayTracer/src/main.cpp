#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>

#include "Engine.h"
#include "Camera.h"
#include "World.h"
#include "shaderBackup.h"

constexpr auto ROOT_DIR = "/home/roboevt/dev/c++/OpenGLRayTracer/OpenGLRayTracer/";

int main() {
    Engine engine = Engine();
    World world = World();

    const std::string vertexCode = ROOT_DIR + std::string("res/shaders/vertex.glsl");
    const std::string rayFragmentCode = ROOT_DIR + std::string("res/shaders/rayTrace.frag");
    const std::string screenFragmentCode = ROOT_DIR + std::string("res/shaders/screen.frag");

    Shader rayShader = Shader(vertexCode, rayFragmentCode);
    Shader screenShader = Shader(vertexCode, screenFragmentCode);

    // Load backup shaders if files are not found
    if (!rayShader) rayShader.create(vertexBackup, rayFragmentBackup);
    if (!screenShader) screenShader.create(vertexBackup, screenFragmentBackup);
    
    engine.setShaders(rayShader, screenShader);

    engine.setWindowSize(1920, 1080);

    world.addSpheres(World::createSphereTestScene());

    // Create mirror plane square
    Triangle triangle1;
    triangle1.a = glm::vec3(-5, 0, -2);
    triangle1.b = glm::vec3(5, 0, -2);
    triangle1.c = glm::vec3(-5, 5, -2);
    triangle1.material.diffuseColor = glm::vec3(0.9, 0.9, 0.9);
    triangle1.material.emissionColor = glm::vec3(0, 0, 0);
    triangle1.material.emissionStrength = 0;
    triangle1.material.reflectivity = 1;

    Triangle triangle2 = triangle1;
    triangle2.a = glm::vec3(5, 5, -2);
    triangle2.b = glm::vec3(-5, 5, -2);
    triangle2.c = glm::vec3(5, 0, -2);

    world.addTriangle(triangle1);
    world.addTriangle(triangle2);

    while (!engine.draw()) {
        engine.update();
        world.applyMVP(engine.getViewMatrix());
        world.updateShader(engine.getShader());
    }
    return 0;
}