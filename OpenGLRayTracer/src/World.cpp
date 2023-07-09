#include "World.h"

#include "stl_reader.h"

void World::updateShaderObjectCount(const std::string& field, const int shader, const int count) const {
	int loc = glGetUniformLocation(shader, field.c_str());
	glUniform1i(loc, count);
}

void World::updateShader(int shader) {
	int i = 0;
	for (const Sphere& sphere : spheres) {
		sphere.sendToShader(shader, i++);
	}
	updateShaderObjectCount("numSpheres", shader, i);
	i = 0;
	for (const Triangle& triangle : triangles) {
		triangle.sendToShader(shader, i++);
		if(i > 128) break;  // TODO find a better way (not uniforms) to transfer triangles to shader
	}
	updateShaderObjectCount("numTriangles", shader, i);\
}

void World::applyMVP(glm::mat4 mvp) {
	for (Sphere& sphere : spheres) {
		sphere.applyMVP(mvp);
	}
	for(Triangle& triangle : triangles) {
		triangle.applyMVP(mvp);
	}
}

void World::addStl(std::string path) {
	stl_reader::StlMesh<float, unsigned int> mesh(path);
	for(size_t i = 0; i < mesh.num_tris(); i++) {
		Triangle triangle;
		const float* v1 = mesh.tri_corner_coords(i, 0);
		const float* v2 = mesh.tri_corner_coords(i, 1);
		const float* v3 = mesh.tri_corner_coords(i, 2);
		triangle.a  = glm::vec3(v1[0], v1[1] + 1, v1[2] + 2);
		triangle.b  = glm::vec3(v2[0], v2[1] + 1, v2[2] + 2);
		triangle.c  = glm::vec3(v3[0], v3[1] + 1, v3[2] + 2);
		triangle.material.diffuseColor = glm::vec3(1);
		triangle.material.emissionColor = glm::vec3(0);
		triangle.material.emissionStrength = 0;
		triangle.material.reflectivity = 0;
		addTriangle(triangle);
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

std::vector<Triangle> World::createTestMirror() {
	    Triangle triangle1;
    triangle1.a = glm::vec3(-5, 0, -1);
    triangle1.b = glm::vec3(5, 0, -1);
    triangle1.c = glm::vec3(-5, 5, -1);
    triangle1.material.diffuseColor = glm::vec3(0.9, 0.9, 0.9);
    triangle1.material.emissionColor = glm::vec3(0, 0, 0);
    triangle1.material.emissionStrength = 0;
    triangle1.material.reflectivity = 1;

    Triangle triangle2 = triangle1;
    triangle2.a = glm::vec3(5, 5, -1);
    triangle2.b = glm::vec3(-5, 5, -1);
    triangle2.c = glm::vec3(5, 0, -1);

	return {triangle1, triangle2};
}