#ifndef RENDERER_H
#define RENDERER_H

#include <iostream>
#include <vector>
#include <glad/glad.h> // 使用 GLAD 来加载 OpenGL 函数
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include "ParticleSystem.h" // 包含粒子系统类的头文件

class Renderer
{
public:
    Renderer();
    Renderer(int width, int height);
    ~Renderer();

    GLFWwindow *getWindow() { return window; }

    void initialize();
    void render(ParticleSystem &particleSystem);

private:
    int windowWidth;
    int windowHeight;
    GLFWwindow *window;
    GLuint particleVBO;
    GLuint particleVAO;
    GLuint shaderProgram;

    // 其他私有成员和渲染相关的函数...
};

#endif // RENDERER_H
