#ifndef PARTICLE_SYSTEM_H
#define PARTICLE_SYSTEM_H

#include <vector>
#include <glm/glm.hpp>

class Particle
{
public:
    glm::vec3 position;
    glm::vec3 velocity;
    glm::vec3 prePosition;
    glm::vec3 preVelpcity;
    glm::vec3 A;
    glm::vec3 F;
    glm::vec3 color;
    float Density;
    float pressure;
    float mass;
    float volume;
    float viscosity;

    Particle(const glm::vec3 &pos, const glm::vec3 &vel) : position(pos), velocity(vel)
    {
    }
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
