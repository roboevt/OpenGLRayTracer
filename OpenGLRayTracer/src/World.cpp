#include "World.h"

void World::updateShaderObjectCount(const std::string& field, int shader) {
	int loc = glGetUniformLocation(shader, field.c_str());
	glUniform1i(loc, spheres.size());
}

void World::updateShader(int shader) {
	int i = 0;
	for (const Sphere& sphere : spheres) {
		sphere.sendToShader(shader, i++);
	}
	updateShaderObjectCount("numSpheres", shader);
	i = 0;
	for (const Triangle& triangle : triangles) {
		triangle.sendToShader(shader, i++);
	}
	updateShaderObjectCount("numTriangles", shader);
}

void World::applyMVP(glm::mat4 mvp) {
	for (Sphere& sphere : spheres) {
		sphere.applyMVP(mvp);
	}
	for(Triangle& triangle : triangles) {
		triangle.applyMVP(mvp);
	}
}

std::vector<Sphere> World::createSphereTestScene() {
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

	return {sphere1, sphere2, sphere3, sphere4, sphere5};
}