#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>

#include <string>

struct Material {
	glm::vec3 color;
};

class Object {
public:
	virtual void sendToShader(int shader, int index) = 0;
	virtual void applyMVP(glm::mat4 mvp) = 0;
	virtual void sendField(int shader, int index, const std::string& name, float value) = 0;
	virtual void sendField(int shader, int index, const std::string& name, glm::vec3 value) = 0;
	//virtual ~Object();
};

struct Sphere : Object {
	glm::vec3 worldLocation, cameraLocation;
	float radius;
	Material material;

	static const std::string shaderBuffer;

	void sendToShader(int shader, int index) override;
	void applyMVP(glm::mat4 mvp) override;

	void sendField(int shader, int index, const std::string& name, float value) override;
	void sendField(int shader, int index, const std::string& name, glm::vec3 value) override;
};
