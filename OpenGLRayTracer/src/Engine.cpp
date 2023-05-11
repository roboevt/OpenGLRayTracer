#include "Engine.h"
#include "shaderBackup.h"

namespace {  // Helper functions that do not need (or can't have in case of callback) access to class members
    unsigned int compileShader(unsigned int type, const std::string& source) {
        unsigned int id = glCreateShader(type);
        const char* src = source.c_str();
        glShaderSource(id, 1, &src, nullptr);
        glCompileShader(id);

        int result;
        glGetShaderiv(id, GL_COMPILE_STATUS, &result);
        if (result == GL_FALSE) {
            int length;
            glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
            char* message = (char*)alloca(length * sizeof(char));  // dynamic stack allocation
            glGetShaderInfoLog(id, length, &length, message);
            std::cout << "Failed to compile " <<
                (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << " shader:" <<
                message << std::endl;
            glDeleteShader(id);
            return 0;
        }

        return id;
    }

    void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
        glViewport(0, 0, width, height);
        Engine* engine = static_cast<Engine*>(glfwGetWindowUserPointer(window));
        engine->updateWindowUniform();
        engine->draw();
    }

    void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
        Engine* engine = static_cast<Engine*>(glfwGetWindowUserPointer(window));
        engine->updateCamera((float)xpos, (float)ypos);
    }

    void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
        if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);  // capture mouse
            glfwSetCursorPosCallback(window, mouse_callback);
        }
    }

    void createFullscreenQuad() {
        float positions[] = {
        -1, -1,
        1, -1,
        1, 1,

        1, 1,
        -1, 1,
        -1, -1
        };

        unsigned int buffer;
        glGenBuffers(1, &buffer);
        glBindBuffer(GL_ARRAY_BUFFER, buffer);
        glBufferData(GL_ARRAY_BUFFER, 6 * 2 * sizeof(float), positions, GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, nullptr);
    }
}

unsigned int Engine::CreateShader(const ShaderProgramSource& source) {
    unsigned int program = glCreateProgram();
    unsigned int vs = compileShader(GL_VERTEX_SHADER, source.VertexSource);
    unsigned int fs = compileShader(GL_FRAGMENT_SHADER, source.FragmentSource);

    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glValidateProgram(program);

    glDeleteShader(vs);
    glDeleteShader(fs);

    return program;
}

Engine::ShaderProgramSource Engine::parseGLSL(const std::string& vertexPath, const std::string& fragmentPath) {
    std::ifstream stream(vertexPath);
    if (stream.is_open()) {
        std::string line;
        std::stringstream ss[2];
        while (getline(stream, line)) { ss[0] << line << '\n'; }
        stream.close();
        stream.open(fragmentPath);
        while (getline(stream, line)) { ss[1] << line << '\n'; }
        return { ss[0].str(), ss[1].str() };
    }
    else return { vertexBackup, fragmentBackup };
}

Engine::ShaderProgramSource Engine::parseShader(const std::string& filePath) {
    std::ifstream stream(filePath);
    enum class ShaderType {
        NONE = -1, VERTEX = 0, FRAGMENT = 1
    };
    std::string line;
    std::stringstream ss[2];
    ShaderType type = ShaderType::NONE;
while (getline(stream, line)) {
    if (line.find("#shader") != std::string::npos) {
        if (line.find("vertex") != std::string::npos) {
            type = ShaderType::VERTEX;
        }
        else if (line.find("fragment") != std::string::npos) {
            type = ShaderType::FRAGMENT;
        }
    }
    else {
        if (type != ShaderType::NONE) ss[(int)type] << line << '\n';
    }
}
return { ss[0].str(), ss[1].str() };
}

void Engine::loadShaders(const std::string& vertexPath, const std::string& fragmentPath) {
    ShaderProgramSource source = parseGLSL(vertexPath, fragmentPath);
    shader = CreateShader(source);
    glUseProgram(shader);
    updateWindowUniform();
}

void Engine::updateWindowUniform() {
    int uResolution = glGetUniformLocation(shader, "uResolution");
    glUniform2f(uResolution, (float)windowWidth, (float)windowHeight);
}

Engine::Engine(int width, int height, const std::string& vertexPath,
    const std::string& fragmentPath) : windowWidth(width), windowHeight(height),
    lastUpdate(std::chrono::high_resolution_clock::now()), lastFrame(std::chrono::high_resolution_clock::now()),
    startTime(std::chrono::high_resolution_clock::now()),
    frames(0), lastFrameTime(0) {

    if (!glfwInit()) throw "GLFW failed to initialize";
    window = glfwCreateWindow(windowWidth, windowHeight, "OpenGL", NULL, NULL);
    if (!window) {
        glfwTerminate();
        throw "Failed to create Window";
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(0); // request no vsync
    glfwSetWindowUserPointer(window, this);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);  // capture mouse
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);

    if (glewInit() != GLEW_OK) throw "GLEW failed to initialize";
    std::cout << "GL Version: " << glGetString(GL_VERSION) << std::endl;

    createFullscreenQuad();
    loadShaders(vertexPath, fragmentPath);
}

Engine::~Engine() {
    glDeleteProgram(shader);
    glfwTerminate();
}

int Engine::draw() {
    glClear(GL_COLOR_BUFFER_BIT);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glfwSwapBuffers(window);

    frames++;
    auto now = std::chrono::high_resolution_clock::now();
    lastFrameTime = std::chrono::duration<float>(now - lastFrame).count();
    lastFrame = now;

    auto elapsedMs = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastUpdate).count();
    if (elapsedMs >= 1000) {
        float fps = frames / ((float)elapsedMs / 1000);
        frames = 0;
        lastUpdate = now;

        //std::cout << "fps: " << fps << std::endl;
        std::string title = "OpenGL, " + std::to_string((int)fps) + "fps";
        glfwSetWindowTitle(window, title.c_str());
    }
    glfwPollEvents();
    return glfwWindowShouldClose(window);
}

void Engine::updateCamera(float newX, float newY) {
    camera.ProcessMouseMovement(lastMouseX - newX, newY - lastMouseY, true);
    lastMouseX = newX;
    lastMouseY = newY;
}

void Engine::update() {
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        camera.ProcessKeyboard(BACKWARD, lastFrameTime);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        camera.ProcessKeyboard(LEFT, lastFrameTime);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        camera.ProcessKeyboard(FORWARD, lastFrameTime);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        camera.ProcessKeyboard(RIGHT, lastFrameTime);
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
        camera.ProcessKeyboard(UP, lastFrameTime);
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
        camera.ProcessKeyboard(DOWN, lastFrameTime);
    }
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        glfwSetCursorPosCallback(window, nullptr);
    }
}

float Engine::getTime() {
    auto now = std::chrono::high_resolution_clock::now();
    return std::chrono::duration<float>(now - startTime).count();
}