#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Engine.h"
#include "Camera.h"
#include "World.h"
#include "shaderBackup.h"

#define VERTEX_SOURCE 2
#define RAY_FRAGMENT_SOURCE 3
#define SCREEN_FRAGMENT_SOURCE 4

int main(int argc, char* argv[]) {
    Engine engine = Engine();

    World world = World();

    Shader rayShader = Shader("res/shaders/vertex.glsl", "res/shaders/rayTrace.frag");
    Shader screenShader = Shader("res/shaders/vertex.glsl", "res/shaders/screen.frag"); 

    if (!rayShader) rayShader.create(vertexBackup, rayFragmentBackup);
    if (!screenShader) screenShader.create(vertexBackup, screenFragmentBackup);
    
    engine.setShaders(rayShader, screenShader);

    engine.setWindowSize(1920, 1080);

    Sphere sphere1;
    sphere1.worldLocation = glm::vec3(2);
    sphere1.radius = .5;
    sphere1.material.diffuseColor = glm::vec3(0);
    sphere1.material.emissionColor = glm::vec3(1);
    sphere1.material.emissionStrength = 16;

    Sphere sphere2;
    sphere2.worldLocation = glm::vec3(0, 1, 6);
    sphere2.radius = 1;
    sphere2.material.diffuseColor = glm::vec3(1);
    sphere2.material.emissionColor = glm::vec3(1);
    sphere2.material.reflectivity = .99;

    Sphere sphere3;
    sphere3.worldLocation = glm::vec3(0, -10000, 6);
    sphere3.radius = 10000;
    sphere3.material.diffuseColor = glm::vec3(.2, .2, .2);
    sphere3.material.emissionColor = glm::vec3(1);

    Sphere sphere4;
    sphere4.worldLocation = glm::vec3(1, .5, 5);
    sphere4.radius = .5;
    sphere4.material.diffuseColor = glm::vec3(1, .2, .2);
    sphere4.material.emissionColor = glm::vec3(1);

    Sphere sphere5;
    sphere5.worldLocation = glm::vec3(2.5, .5, 4.4);
    sphere5.radius = .5;
    sphere5.material.diffuseColor = glm::vec3(.3, .4, .9);
    sphere5.material.emissionColor = glm::vec3(1);

    world.addObject(std::make_unique<Sphere>(sphere1));
    world.addObject(std::make_unique<Sphere>(sphere2));
    world.addObject(std::make_unique<Sphere>(sphere3));
    world.addObject(std::make_unique<Sphere>(sphere4));
    world.addObject(std::make_unique<Sphere>(sphere5));

    while (!engine.draw()) {
        engine.update();
        world.applyMVP(engine.getViewMatrix());
        world.updateShader(engine.getShader());
    }
    return 0;
}