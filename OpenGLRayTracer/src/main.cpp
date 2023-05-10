#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <chrono>

#define WIDTH 640
#define HEIGHT 480

struct ShaderProgramSource {
    std::string VertexSource;
    std::string FragmentSource;
};

static ShaderProgramSource parseShader(const std::string& filePath) {
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

static ShaderProgramSource parseGLSL(const std::string& vertexPath, const std::string& fragmentPath) {
    std::ifstream stream(vertexPath);
    std::string line;
    std::stringstream ss[2];
    while (getline(stream, line)) { ss[0] << line << '\n'; }
    stream.close();
    stream.open(fragmentPath);
    while (getline(stream, line)) { ss[1] << line << '\n'; }
    return { ss[0].str(), ss[1].str() };
}

static unsigned int compileShader(unsigned int type, const std::string& source) {
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

static unsigned int CreateShader(const std::string& vertextShader, const std::string& fragmentShader) {
    unsigned int program = glCreateProgram();
    unsigned int vs = compileShader(GL_VERTEX_SHADER, vertextShader);
    unsigned int fs = compileShader(GL_FRAGMENT_SHADER, fragmentShader);

    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glValidateProgram(program);

    glDeleteShader(vs);
    glDeleteShader(fs);

    return program;
}

static GLFWwindow* window;
static unsigned int shader;
static int frames;

static void draw() {
    frames++;
    glClear(GL_COLOR_BUFFER_BIT);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glfwSwapBuffers(window);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
    int uResolution = glGetUniformLocation(shader, "uResolution");
    glUniform2f(uResolution, (float)width, (float)height);

    draw();
}

int main(void)
{
    if (!glfwInit()) return -1;
    window = glfwCreateWindow(WIDTH, HEIGHT, "OpenGL", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(0); // request no vsync
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (glewInit() != GLEW_OK) { std::cout << "GLEW Init error\n";}

    std::cout << "GL Version: " << glGetString(GL_VERSION) << std::endl;
    
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

    //ShaderProgramSource source = parseShader("res/shaders/Basic.shader");
    ShaderProgramSource source = parseGLSL("res/shaders/vertex.glsl", "res/shaders/fragment.glsl");
    shader = CreateShader(source.VertexSource, source.FragmentSource);
    glUseProgram(shader);

    int uResolution = glGetUniformLocation(shader, "uResolution");
    glUniform2f(uResolution, (float)WIDTH, (float)HEIGHT);

    frames = 0;
    auto begin = std::chrono::steady_clock::now();
    while (!glfwWindowShouldClose(window)) {
        
        auto now = std::chrono::steady_clock::now();
        auto elapsedMs = std::chrono::duration_cast<std::chrono::milliseconds>(now - begin).count();
        if (elapsedMs >= 1000) {
            float fps = frames / ((float)elapsedMs / 1000);
            std::cout << "fps: " << fps << std::endl;
            frames = 0;
            begin = std::chrono::steady_clock::now();
        }
        
        draw();

        glfwPollEvents();
    }

    glDeleteProgram(shader);
    glfwTerminate();
    return 0;
}