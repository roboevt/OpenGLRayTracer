#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <chrono>


class Engine
{
public:  // Should probably be private in the future, but this helps for testing/dev
	GLFWwindow* window;
	unsigned int shader;
	int windowWidth, windowHeight;
	struct ShaderProgramSource {
		std::string VertexSource;
		std::string FragmentSource;
	};
	std::chrono::time_point<std::chrono::high_resolution_clock> startTime, lastUpdate, lastFrame;
	float lastFrameTime;
	int frames;

	void loadShaders(const std::string& vertexPath, const std::string& fragmentPath);
	ShaderProgramSource parseGLSL(const std::string& vertexPath, const std::string& fragmentPath);
	static ShaderProgramSource parseShader(const std::string& filePath);
	unsigned int CreateShader(const ShaderProgramSource& source);

public:
	Engine(int width = 640, int height = 480, const std::string& vertexPath = "res/shaders/vertex.glsl",
		const std::string& fragmentPath = "res/shaders/fragment.glsl");
	~Engine();
	int draw();
	void updateWindowUniform();
	float getLastFrametime() { return lastFrameTime; }
	float getTime();
};

