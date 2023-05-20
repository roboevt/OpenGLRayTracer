#include "Engine.h"

#include <iostream>
#include <fstream>
#include <sstream>

#include <chrono>
#include <thread>

namespace {  // Helper functions that do not need (or can't have in case of callback) access to class members

    void GLAPIENTRY
        MessageCallback(GLenum source, GLenum type, GLuint id,
            GLenum severity, GLsizei length, const GLchar* message, const void* userParam) {
        if (severity == GL_DEBUG_SEVERITY_LOW || severity == GL_DEBUG_SEVERITY_MEDIUM || severity == GL_DEBUG_SEVERITY_HIGH) {
            fprintf(stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
                (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
                type, severity, message);
        }
    }

    void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
        Engine* engine = static_cast<Engine*>(glfwGetWindowUserPointer(window));
        engine->setWindowSize(width, height);
    }

    void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
        Engine* engine = static_cast<Engine*>(glfwGetWindowUserPointer(window));
        engine->moveCamera((float)xpos, (float)ypos);
    }

    void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
        if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);  // capture mouse
            glfwSetCursorPosCallback(window, mouse_callback);
        }
    }

    void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
        Engine* engine = static_cast<Engine*>(glfwGetWindowUserPointer(window));
        if (key == GLFW_KEY_EQUAL && action == GLFW_PRESS) {
            engine->setSamples(engine->getSamples() * 2);
        }
        if (key == GLFW_KEY_MINUS && action == GLFW_PRESS) {
            engine->setSamples(engine->getSamples() / 2);
        }
    }

    void createFullscreenQuad() {
        float positions[] = {
        -1, -1,
        1, -1,
        1, 1,

        1, 1,
        -1, 1,
        -1, -1 };

        unsigned int buffer;
        glGenBuffers(1, &buffer);
        glBindBuffer(GL_ARRAY_BUFFER, buffer);
        glBufferData(GL_ARRAY_BUFFER, 6 * 2 * sizeof(float), positions, GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, nullptr);
    }
}

Engine::Engine(int width, int height, int samples) : windowWidth(width), windowHeight(height), samples(samples), 
    lastUpdateTime(std::chrono::high_resolution_clock::now()), lastFrameTime(std::chrono::high_resolution_clock::now()),
    startTime(std::chrono::high_resolution_clock::now()),
    frames(0), lastFrameDuration(0) {

    if (!glfwInit()) throw "GLFW failed to initialize";
    window = glfwCreateWindow(windowWidth, windowHeight, "OpenGL", NULL, NULL);
    if (!window) {
        glfwTerminate();
        throw "Failed to create Window";
    }

    glfwMakeContextCurrent(window);
    //glfwSwapInterval(0); // request no vsync
    glfwSetWindowUserPointer(window, this);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);  // capture mouse
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetKeyCallback(window, key_callback);

    if (glewInit() != GLEW_OK) throw "GLEW failed to initialize";
    std::cout << "GL Version: " << glGetString(GL_VERSION) << std::endl;

    glEnable(GL_DEBUG_OUTPUT);  // debug messages
    glDebugMessageCallback(MessageCallback, 0);

    createFullscreenQuad();
    moveCamera(0, 0);
    createFBO();
}

int Engine::createFBO() {
    glCreateFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    glCreateTextures(GL_TEXTURE_2D, 1, &bufferTexture); 
    glBindTexture(GL_TEXTURE_2D, bufferTexture);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, windowWidth, windowHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bufferTexture, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "Error creating framebuffer with status: "
            << glCheckFramebufferStatus(GL_FRAMEBUFFER) << std::endl;
        return 1;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glClear(GL_COLOR_BUFFER_BIT);

    return 0;
}

void Engine::setShaders(Shader rayShader, Shader screenShader) {
    this->rayShader = rayShader;
    this->screenShader = screenShader;
}

int Engine::draw() {
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glUseProgram(rayShader);
    glDrawArrays(GL_TRIANGLES, 0, 6);  // draw rendered image to buffer texture

    glUseProgram(screenShader);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClear(GL_COLOR_BUFFER_BIT);
    glDrawArrays(GL_TRIANGLES, 0, 6);  // average out buffer and display to screen

    glfwSwapBuffers(window);
    glfwPollEvents();
    return glfwWindowShouldClose(window);
}

void Engine::clearBuffer() {
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glClear(GL_COLOR_BUFFER_BIT);
}

void Engine::moveCamera(float newX, float newY) {
    bool anyMovement = newX != lastMouseX || newY != lastMouseY;
    camera.ProcessMouseMovement(lastMouseX - newX, newY - lastMouseY, true);
    lastMouseX = newX;
    lastMouseY = newY;
    if (anyMovement) {
        frames = 1;
        clearBuffer();
    }
}

void Engine::updateKeyboard() {
    bool anyMovement = false;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        camera.ProcessKeyboard(BACKWARD, lastFrameDuration);
        anyMovement = true;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        camera.ProcessKeyboard(LEFT, lastFrameDuration);
        anyMovement = true;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        camera.ProcessKeyboard(FORWARD, lastFrameDuration);
        anyMovement = true;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        camera.ProcessKeyboard(RIGHT, lastFrameDuration);
        anyMovement = true;
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
        camera.ProcessKeyboard(UP, lastFrameDuration);
        anyMovement = true;
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
        camera.ProcessKeyboard(DOWN, lastFrameDuration);
        anyMovement = true;
    }
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        glfwSetCursorPosCallback(window, nullptr);
    }
    if (anyMovement) {
        frames = 1;
        clearBuffer();
    }
}

void Engine::updateUniforms() {
    glUseProgram(rayShader);

    int loc = glGetUniformLocation(rayShader, "samples");
    glUniform1i(loc, samples);

    loc = glGetUniformLocation(rayShader, "samples");
    glUniform1i(loc, samples);

    loc = glGetUniformLocation(rayShader, "mvp");
    glUniformMatrix4fv(loc, 1, false, &camera.GetViewMatrix()[0][0]);

    loc = glGetUniformLocation(rayShader, "frames");
    glUniform1ui(loc, frames);
    
    glUseProgram(screenShader);

    loc = glGetUniformLocation(screenShader, "frames");
    glUniform1ui(loc, frames);

    glUseProgram(rayShader);
}

void Engine::updateFPS() {
    frames++;
    auto now = std::chrono::high_resolution_clock::now();
    lastFrameDuration = std::chrono::duration<float>(now - lastFrameTime).count();
    lastFrameTime = now;

    auto elapsedMs = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastUpdateTime).count();
    if (elapsedMs >= 1000) {
        float fps = (frames - lastFrames) / ((float)elapsedMs / 1000);
        lastFrames = frames;
        lastUpdateTime = now;

        //std::cout << "fps: " << fps << std::endl;
        std::string title = "OpenGL, " + std::to_string((int)fps) + "fps";
        glfwSetWindowTitle(window, title.c_str());
    }
}

void Engine::update() {
    updateKeyboard();
    updateUniforms();
    updateFPS();
}

float Engine::getLastFrametime() { return lastFrameDuration; }

float Engine::getTime() {
    auto now = std::chrono::high_resolution_clock::now();
    return std::chrono::duration<float>(now - startTime).count();
}

glm::mat4 Engine::getViewMatrix() { return camera.GetViewMatrix(); }

int Engine::getShader() { return rayShader; }

int Engine::getSamples() { return samples; }

void Engine::setSamples(int samples) { if(samples) this->samples = samples; }

void Engine::setWindowSize(int width, int height) {
    this->windowWidth = width;
    this->windowHeight = height;
    glViewport(0, 0, width, height);
    
    glUseProgram(rayShader);
    int loc = glGetUniformLocation(rayShader, "uResolution");
    glUniform2f(loc, (float)windowWidth, (float)windowHeight);
    glUseProgram(screenShader);
    loc = glGetUniformLocation(screenShader, "uResolution");
    glUniform2f(loc, (float)windowWidth, (float)windowHeight);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, windowWidth, windowHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

    draw();
}

Engine::~Engine() {
    glfwTerminate();
}