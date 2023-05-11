#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Engine.h"
#include "Camera.h"
#include "World.h"

int main(void) {
    Engine engine = Engine();

    World world = World();

    Sphere sphere1;
    sphere1.worldLocation = glm::vec3(0, 0, 6);
    sphere1.radius = 1;
    sphere1.material.color = glm::vec3(1, .3, 1);

    world.addObject(std::make_unique<Sphere>(sphere1));


    Sphere sphere2;
    sphere2.worldLocation = glm::vec3(0, 1, 6);
    sphere2.radius = 1;
    sphere2.material.color = glm::vec3(.2, .3, 1);
    
    world.addObject(std::make_unique<Sphere>(sphere2));

    while (!engine.draw()) {
        engine.update();
        //engine.camera.Position.y = sin(engine.getTime());
        world.applyMVP(engine.camera.GetViewMatrix());
        world.updateShader(engine.shader);
    }
    return 0;
}