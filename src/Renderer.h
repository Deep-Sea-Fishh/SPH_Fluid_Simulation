#ifndef RENDERER_H
#define RENDERER_H

#include <iostream>
#include <vector>
#include <glad/glad.h> // 使用 GLAD 来加载 OpenGL 函数
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include "shader.h"
#include "fluidSystem.h"
#include "camera.h"
// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

class Renderer
{
public:
    Renderer();
    Renderer(int width, int height);
    ~Renderer();

    GLFWwindow *getWindow() { return window; }
    void setBuffer(void *buffer, int size, int type, void *idxBuf = nullptr, int idxSz = 0);

    void initialize();
    void setShader(int type, const char *vertexShader, const char *fragmentShader);
    void render();

private:
    int windowWidth;
    int windowHeight;
    GLFWwindow *window;
    GLuint particleVBO;
    GLuint particleVAO;
    GLuint particleEBO;
    GLuint boxVBO;
    GLuint boxVAO;
    GLuint boxEBO;
    GLuint gridVBO;
    GLuint gridVAO;
    GLuint gridEBO;
    Shader *shader;
    Shader *sufShader;
    int pointSize;
    int posSize;
    int triSize;
    // 其他私有成员和渲染相关的函数...
};

#endif // RENDERER_H
