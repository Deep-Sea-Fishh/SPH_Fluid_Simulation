#include "Renderer.h"
#include "ParticleSystem.h"
#include <iostream>

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
}

int main()
{

    ParticleSystem particleSystem(10);

    Renderer *renderer = new Renderer(800, 600);
    renderer->initialize();
    while (!glfwWindowShouldClose(renderer->getWindow()))
    {
        renderer->render(particleSystem);
    }
    return 0;
}