#pragma once

#include <vector>
#include <memory>

#include "Object.h"

struct World {
	std::vector<std::unique_ptr<Object>> objects;
	void addObject(std::unique_ptr<Object> object) { objects.emplace_back(std::move(object)); }

	void updateShader(int shader);
	void applyMVP(glm::mat4 mvp);

private:
	void updateShaderObjectCount(int shader);
};

