#include "Renderer.h"
#include <GLFW/glfw3.h>

Renderer::Renderer() : windowWidth(800), windowHeight(600), window(nullptr), particleVBO(0), particleVAO(0) {}
Renderer::Renderer(int width, int height) : windowWidth(width), windowHeight(height), window(nullptr), particleVBO(0), particleVAO(0) {}
Renderer::~Renderer()
{
    glDeleteBuffers(1, &particleVBO);
    glDeleteVertexArrays(1, &particleVAO);
    glfwDestroyWindow(window);
    glfwTerminate();
}

void Renderer::initialize()
{
    if (!glfwInit())
    {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

    window = glfwCreateWindow(windowWidth, windowHeight, "Fluid Simulation Renderer", nullptr, nullptr);
    if (!window)
    {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return;
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return;
    }
}

void Renderer::render(ParticleSystem &particleSystem)
{
    glClear(GL_COLOR_BUFFER_BIT);

    glfwSwapBuffers(window);
    glfwPollEvents();
}

// 其他渲染相关的函数实现...
