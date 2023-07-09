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
    world.addTriangles(World::createTestMirror());

    world.addStl("basic.stl");

    while (!engine.draw()) {
        engine.update();
        world.applyMVP(engine.getViewMatrix());
        world.updateShader(engine.getShader());
    }
    return 0;
}