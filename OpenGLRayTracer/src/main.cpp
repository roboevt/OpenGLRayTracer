#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Engine.h"
#include "Camera.h"
#include "Sphere.h"

int main(void) {
    Engine engine = Engine();

    Camera camera = Camera();

    Sphere sphere1;
    sphere1.location = glm::vec3(0, 0, 6);
    sphere1.radius = 1;
    sphere1.material.color = glm::vec3(1, .3, 1);
    sphere1.sendToShader(engine.shader, 0);

    Sphere sphere2;
    sphere2.location = glm::vec3(0, 1, 6);
    sphere2.radius = 1;
    sphere2.material.color = glm::vec3(.2, .3, 1);
    sphere2.sendToShader(engine.shader, 1);

    while (!engine.draw()) {
        camera.Position.y = sin(engine.getTime() / 1) / 2000;
        sphere1.applyMVP(camera.GetViewMatrix());
        sphere2.applyMVP(camera.GetViewMatrix());
        sphere1.sendToShader(engine.shader, 0);
        sphere2.sendToShader(engine.shader, 1);
    }
    return 0;
}