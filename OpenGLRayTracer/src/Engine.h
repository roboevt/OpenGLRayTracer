#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <string>
#include <chrono>

#include "Shader.h"
#include "Camera.h"


class Engine {
private:
	GLFWwindow* window;
	int windowWidth, windowHeight;

	unsigned int fbo, bufferTexture;

	std::chrono::time_point<std::chrono::high_resolution_clock> startTime, lastUpdateTime, lastFrameTime;

	float lastMouseX, lastMouseY;
	float lastFrameDuration;
	unsigned int frames, lastFrames;
	int samples;

	Camera camera;
	Shader rayShader, screenShader;

	int createFBO();

	void updateKeyboard();
	void updateUniforms();
	void updateFPS();

	void clearBuffer();
public:
	Engine(int width = 1920, int height = 1080, int samples = 16);
	void setShaders(Shader rayShader, Shader screenShader);
	~Engine();
	int draw();
	void update();
	void moveCamera(float newX, float newY);
	float getLastFrametime();
	float getTime();
	glm::mat4 getViewMatrix();
	int getShader();
	int getSamples();
	void setSamples(int samples);
	void setWindowSize(int width, int height);
};

