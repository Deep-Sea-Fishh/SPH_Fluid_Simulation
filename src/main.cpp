#include <iostream>
#include "fluidSystem.h"
#include "Renderer.h"
#include "setting.h"
const char *vertexShader = "../shader/point.vs";
const char *fragmentShader = "../shader/point.fs";
const char *sufVertexShader = "../shader/suf.vs";
const char *sufFragmentShader = "../shader/suf.fs";
int main()
{
    float *ptBuffer;

    FluidSystem fluidSystem;
    fluidSystem.init(65535, wallBoxMin, wallBoxMax, fluidBoxMin, fluidBoxMax, gravity);
    std::cout << 1 << std::endl;
    Renderer *renderer = new Renderer(SCR_WIDTH, SCR_HEIGHT);
    renderer->initialize();
    renderer->setShader(0, vertexShader, fragmentShader);
    renderer->setShader(1, sufVertexShader, sufFragmentShader);
    // tell GLFW to capture our mouse
    glfwSetInputMode(renderer->getWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // 包围盒
    float box[24] = {
        wallBoxMin.x,
        wallBoxMin.y,
        wallBoxMin.z,

        wallBoxMin.x,
        wallBoxMin.y,
        wallBoxMax.z,

        wallBoxMin.x,
        wallBoxMax.y,
        wallBoxMin.z,

        wallBoxMin.x,
        wallBoxMax.y,
        wallBoxMax.z,

        wallBoxMax.x,
        wallBoxMin.y,
        wallBoxMin.z,

        wallBoxMax.x,
        wallBoxMin.y,
        wallBoxMax.z,

        wallBoxMax.x,
        wallBoxMax.y,
        wallBoxMin.z,

        wallBoxMax.x,
        wallBoxMax.y,
        wallBoxMax.z,
    };
    while (!glfwWindowShouldClose(renderer->getWindow()))
    {
        for (int i = 0; i < 50; i++)
            fluidSystem.tick();
        auto posData = fluidSystem.getPosData();
        renderer->setBuffer((void *)posData.data(), sizeof(float) * posData.size(), 0);
        renderer->setBuffer((void *)box, sizeof(float) * 24, 1);
#ifdef SUF
        auto verts = fluidSystem.getSufPosNorBuf();
        int verts_sz = fluidSystem.getSufVrtBufNum();
        unsigned int *idxBuf = fluidSystem.getSufTriIdxBuf();
        int idx_sz = fluidSystem.getPolyNum();
        renderer->setBuffer((void *)verts, sizeof(float) * verts_sz * 6, 2, (void *)idxBuf, idx_sz * sizeof(unsigned int));
#endif
        renderer->render();
    }
    return 0;
}
