#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>

#include <string>

struct Material {
	glm::vec3 color;
};

namespace {
	void sendField(int shader, int index, const std::string& name, float value) {
		std::string locationName = "spheres[" + std::to_string(index) + "]." +name;
		int loc = glGetUniformLocation(shader, locationName.c_str());
		glUniform1f(loc, value);
	}
	void sendField(int shader, int index, const std::string& name, glm::vec3 value) {
		std::string locationName = "spheres[" + std::to_string(index) + "]." + name;
		int loc = glGetUniformLocation(shader, locationName.c_str());
		glUniform3f(loc, value.x, value.y, value.z);
	}
}

struct Sphere {
	glm::vec3 location;
	float radius;
	Material material;

	void sendToShader(int shader, int index) {
		sendField(shader, index, "location", location);
		sendField(shader, index, "radius", radius);
		sendField(shader, index, "color", material.color);
	}

	void applyMVP(glm::mat4 mvp) {
		location = mvp * glm::vec4(location, 1);
	}
};
