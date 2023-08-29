#ifndef SETTING_H
#define SETTING_H
#include <glm/glm.hpp>
glm::vec3 wallBoxMin(0, 0, 0);
glm::vec3 wallBoxMax(1, 1, 1);
glm::vec3 fluidBoxMin(0.2f, 0.3f, 0.2f);
glm::vec3 fluidBoxMax(0.9f, 0.8f, 0.9f);
glm::vec3 gravity(0, -9.8, 0);
#endif