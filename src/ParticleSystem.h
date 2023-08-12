#ifndef PARTICLE_SYSTEM_H
#define PARTICLE_SYSTEM_H

#include <vector>
#include <glm/glm.hpp>

class Particle
{
public:
    glm::vec3 position;
    glm::vec3 velocity;
    // 其他粒子属性...

    Particle(const glm::vec3 &pos, const glm::vec3 &vel) : position(pos), velocity(vel) {}
};

class ParticleSystem
{
public:
    std::vector<Particle> particles;

    ParticleSystem(int numParticles){};
    void update(float deltaTime);

private:
    // 其他私有成员和粒子模拟相关的方法...
};

#endif // PARTICLE_SYSTEM_H
