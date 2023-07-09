#include "Object.h"


const std::string Sphere::shaderBuffer = "spheres";
const std::string Triangle::shaderBuffer = "triangles";

void Sphere::sendField(int shader, int index, const std::string& name, float value) const {
	std::string locationName = Sphere::shaderBuffer + "[" + std::to_string(index) + "]." + name;
	int loc = glGetUniformLocation(shader, locationName.c_str());
	glUniform1f(loc, value);
}

void Sphere::sendField(int shader, int index, const std::string& name, glm::vec3 value) const {
	std::string locationName = Sphere::shaderBuffer + "[" + std::to_string(index) + "]." + name;
	int loc = glGetUniformLocation(shader, locationName.c_str());
	glUniform3f(loc, value.x, value.y, value.z);
}

void Sphere::sendToShader(int shader, int index) const {
	sendField(shader, index, "location", cameraLocation);
	sendField(shader, index, "radius", radius);
	sendField(shader, index, "diffuseColor", material.diffuseColor);
	sendField(shader, index, "emissionColor", material.emissionColor);
	sendField(shader, index, "emissionStrength", material.emissionStrength);
	sendField(shader, index, "reflectivity", material.reflectivity);
}

void Sphere::applyMVP(glm::mat4 mvp) {
	cameraLocation = mvp * glm::vec4(worldLocation, 1);
}




void Triangle::sendField(int shader, int index, const std::string& name, float value) const {
	std::string locationName = Triangle::shaderBuffer + "[" + std::to_string(index) + "]." + name;
	int loc = glGetUniformLocation(shader, locationName.c_str());
	glUniform1f(loc, value);
}

void Triangle::sendField(int shader, int index, const std::string& name, glm::vec3 value) const {
	std::string locationName = Triangle::shaderBuffer + "[" + std::to_string(index) + "]." + name;
	int loc = glGetUniformLocation(shader, locationName.c_str());
	glUniform3f(loc, value.x, value.y, value.z);
}

void Triangle::sendToShader(int shader, int index) const {
	sendField(shader, index, "a", cA);
	sendField(shader, index, "b", cB);
	sendField(shader, index, "c", cC);
	sendField(shader, index, "diffuseColor", material.diffuseColor);
	sendField(shader, index, "emissionColor", material.emissionColor);
	sendField(shader, index, "emissionStrength", material.emissionStrength);
	sendField(shader, index, "reflectivity", material.reflectivity);
}

void Triangle::applyMVP(glm::mat4 mvp) {
	cA = mvp * glm::vec4(a, 1);
	cB = mvp * glm::vec4(b, 1);
	cC = mvp * glm::vec4(c, 1);
}