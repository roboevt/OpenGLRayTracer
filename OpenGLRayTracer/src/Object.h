#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>

#include <string>

struct Material {
	glm::vec3 diffuseColor;
	glm::vec3 emissionColor;
	float emissionStrength;
	float reflectivity;
	Material() : diffuseColor(0), emissionColor(0), emissionStrength(0), reflectivity(0) {}
};

class Object {
public:
	virtual void sendToShader(int shader, int index) const = 0;
	virtual void applyMVP(glm::mat4 mvp) = 0;
	virtual void sendField(int shader, int index, const std::string& name, float value) const = 0;
	virtual void sendField(int shader, int index, const std::string& name, glm::vec3 value) const = 0;
};

struct Sphere : Object {
	glm::vec3 cameraLocation, worldLocation;;
	float radius;
	Material material;
	static const std::string shaderBuffer;

	Sphere() : cameraLocation(0), worldLocation(0), radius(1), material() {}

	void sendToShader(int shader, int index) const override;
	void applyMVP(glm::mat4 mvp) override;

	void sendField(int shader, int index, const std::string& name, float value) const override;
	void sendField(int shader, int index, const std::string& name, glm::vec3 value) const override;
};

struct Triangle : Object {
	glm::vec3 a, b, c;
	glm::vec3 cA, cB, cC;
	Material material;
	static const std::string shaderBuffer;

	Triangle() : a(0), b(0), c(0), cA(0), cB(0), cC(0), material() {}

	void sendToShader(int shader, int index) const override;
	void applyMVP(glm::mat4 mvp) override;

	void sendField(int shader, int index, const std::string& name, float value) const override;
	void sendField(int shader, int index, const std::string& name, glm::vec3 value) const override;
};
