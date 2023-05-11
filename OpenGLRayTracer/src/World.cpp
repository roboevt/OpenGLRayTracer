#include "World.h"

void World::updateShaderObjectCount(int shader) {
	int loc = glGetUniformLocation(shader, "numSpheres");
	glUniform1i(loc, objects.size());
}

void World::updateShader(int shader) {
	int i = 0;
	for (const std::unique_ptr<Object>& object : objects) {
		object->sendToShader(shader, i++);
	}
	updateShaderObjectCount(shader);
}

void World::applyMVP(glm::mat4 mvp) {
	for (const std::unique_ptr<Object>& object : objects) {
		object->applyMVP(mvp);
	}
}