#pragma once

#include <memory>
#include <vector>

#include "Object.h"

class World {
   public:
    std::vector<Sphere> spheres;
    void addSphere(Sphere sphere) { spheres.push_back(sphere); }
    void addSpheres(std::vector<Sphere> spheres) {
        this->spheres.insert(this->spheres.end(), spheres.begin(), spheres.end());
    }
	std::vector<Triangle> triangles;
	void addTriangle(Triangle triangle) { triangles.push_back(triangle); }
	void addTriangles(std::vector<Triangle> triangles) {
		this->triangles.insert(this->triangles.end(), triangles.begin(), triangles.end());
	}

	void addStl(std::string path);

    void updateShader(int shader);
    void applyMVP(glm::mat4 mvp);

    static std::vector<Sphere> createSphereTestScene();
	static std::vector<Triangle> createTestMirror();

   private:
    void updateShaderObjectCount(const std::string& field, const int shader, const int count) const;
};
