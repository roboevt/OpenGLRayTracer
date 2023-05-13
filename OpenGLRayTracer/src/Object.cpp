#include "Object.h"


const std::string Sphere::shaderBuffer = "spheres";

void Sphere::sendField(int shader, int index, const std::string& name, float value) {
	std::string locationName = Sphere::shaderBuffer + "[" + std::to_string(index) + "]." + name;
	int loc = glGetUniformLocation(shader, locationName.c_str());
	glUniform1f(loc, value);
}

void Sphere::sendField(int shader, int index, const std::string& name, glm::vec3 value) {
	std::string locationName = Sphere::shaderBuffer + "[" + std::to_string(index) + "]." + name;
	int loc = glGetUniformLocation(shader, locationName.c_str());
	glUniform3f(loc, value.x, value.y, value.z);
}

void Sphere::sendToShader(int shader, int index) {
	sendField(shader, index, "location", cameraLocation);
	sendField(shader, index, "radius", radius);
	sendField(shader, index, "diffuseColor", material.diffuseColor);
	sendField(shader, index, "emissionColor", material.emissionColor);
	sendField(shader, index, "emissionStrength", material.emissionStrength);
}

void Sphere::applyMVP(glm::mat4 mvp) {
	cameraLocation = mvp * glm::vec4(worldLocation, 1);
}