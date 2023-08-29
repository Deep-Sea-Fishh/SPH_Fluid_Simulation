#ifndef POINT_H
#define POINT_H

#include <glm/glm.hpp>

const int ELEM_MAX = 1e7;

struct Point
{
    glm::vec3 pos; // 点位置
    glm::vec3 predicted_position;
    float density; // 密度
    float predicted_density;
    float density_error;
    float pressure;           // 压强
    glm::vec3 pressure_force; // 压力
    glm::vec3 accel;          // 加速度
    glm::vec3 velocity;       // 速度
    glm::vec3 predicted_velocity;
    glm::vec3 velocity_eval; // 最后速度
    glm::vec3 force;         // 受力
    glm::vec3 sum_grad_w;
    float sum_grad_w_dot;
    float kernel_self;

    int next; // 指向下一个点的索引
};

class PointBuffer
{
public:
    PointBuffer();
    void reset(unsigned int capacity);                  // 重置粒子点缓冲
    unsigned int size() const { return m_fluidCounts; } // 返回粒子个数

    // 获取索引为index的点
    Point *get(unsigned int index) { return m_FluidBuf + index; }
    const Point *get(unsigned int index) const { return m_FluidBuf + index; }

    Point *addPointReuse(); // 缓冲中加入新的点并返回
    virtual ~PointBuffer();

private:
    Point *m_FluidBuf;          // 粒子点缓存
    unsigned int m_fluidCounts; // 粒子个数
    unsigned int m_BufCapcity;  // 粒子容量
};

#endif // POINT_H
