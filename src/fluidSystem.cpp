#include "fluidSystem.h"
#include <thread>
#include <cstring>
#include <iostream>
const float eps = 1e-6;
int tickCnt = 0;
FluidSystem::FluidSystem()
{
    m_unitScale = 1.0f;      // 尺寸单位
    m_viscosity = 0.5f;      // 粘度
    m_restDensity = 1000.0f; // 密度
    m_pointMass = 0.1f;      // 粒子质量
    m_gasConstantK = 1.0f;   // 理想气体方程常量
    m_smoothRadius = 0.1f;   // 光滑核半径
    m_pointDistance = 0.05f;

    m_rexSize[0] = 0;
    m_rexSize[1] = 0;
    m_rexSize[2] = 0;

    m_boundaryStiffness = 550.0f;
    m_boundaryDampening = 150.0f;
    m_speedLimiting = 0;

    m_thre = 300;

    // Poly6 Kernel
    m_kernelPoly6 = 315.0f / (64.0f * 3.141592f * pow(m_smoothRadius, 9));
    // Spiky Kernel
    m_kernelSpiky = -45.0f / (3.141592f * pow(m_smoothRadius, 6));
    // Viscosity Kernel
    m_kernelViscosity = 45.0f / (3.141592f * pow(m_smoothRadius, 6));

    threadCnt = 12;
}

FluidSystem::~FluidSystem()
{
}
// 构造流体中的点
void FluidSystem::_addFluidVolume(const fBox3 &fluidBox, float spacing)
{
    int cnt = 0;
    for (float z = fluidBox.max.z; z >= fluidBox.min.z; z -= spacing)
    {
        for (float y = fluidBox.min.y; y <= fluidBox.max.y; y += spacing)
        {
            for (float x = fluidBox.min.x; x <= fluidBox.max.x; x += spacing)
            {
                Point *p = m_pointBuffer.addPointReuse();
                p->pos = glm::vec3(x, y, z);
                p->type = 0;
                cnt++;
                // std::cout << cnt << std::endl;
            }
        }
    }
    // m_sphWallBox;
    // // 前后
    // for (float x = m_sphWallBox.min.x + spacing; x < m_sphWallBox.max.x; x += spacing)
    // {
    //     for (float y = m_sphWallBox.min.y + spacing; y < m_sphWallBox.max.y; y += spacing)
    //     {
    //         Point *p = m_wallBuffer.addPointReuse();
    //         p->pos = glm::vec3(x, y, m_sphWallBox.min.z);
    //         p->type = 1;
    //         p = m_wallBuffer.addPointReuse();
    //         p->pos = glm::vec3(x, y, m_sphWallBox.max.z);
    //         p->type = 1;
    //     }
    // }
    // // 左右
    // for (float z = m_sphWallBox.min.z + spacing; z < m_sphWallBox.max.z; z += spacing)
    // {
    //     for (float y = m_sphWallBox.min.y + spacing; y < m_sphWallBox.max.y; y += spacing)
    //     {
    //         Point *p = m_wallBuffer.addPointReuse();
    //         p->pos = glm::vec3(m_sphWallBox.min.x, y, z);
    //         p->type = 1;
    //         p = m_wallBuffer.addPointReuse();
    //         p->pos = glm::vec3(m_sphWallBox.max.x, y, z);
    //         p->type = 1;
    //     }
    // }
    // // 上下
    // for (float x = m_sphWallBox.min.x; x < m_sphWallBox.max.x; x += spacing)
    // {
    //     for (float z = m_sphWallBox.min.z + spacing; z < m_sphWallBox.max.z; z += spacing)
    //     {
    //         Point *p = m_wallBuffer.addPointReuse();
    //         p->pos = glm::vec3(x, m_sphWallBox.min.y, z);
    //         p->type = 1;
    //         p = m_wallBuffer.addPointReuse();
    //         p->pos = glm::vec3(x, m_sphWallBox.max.y, z);
    //         p->type = 1;
    //     }
    // }
    // for (float y = m_sphWallBox.min.y; y <= m_sphWallBox.max.y; y += spacing)
    // {
    //     Point *p = m_wallBuffer.addPointReuse();
    //     p->pos = glm::vec3(m_sphWallBox.min.x, y, m_sphWallBox.min.z);
    //     p->type = 1;
    //     p = m_wallBuffer.addPointReuse();
    //     p->pos = glm::vec3(m_sphWallBox.min.x, y, m_sphWallBox.max.z);
    //     p->type = 1;
    //     p = m_wallBuffer.addPointReuse();
    //     p->pos = glm::vec3(m_sphWallBox.max.x, y, m_sphWallBox.min.z);
    //     p->type = 1;
    //     p = m_wallBuffer.addPointReuse();
    //     p->pos = glm::vec3(m_sphWallBox.max.x, y, m_sphWallBox.max.z);
    //     p->type = 1;
    // }
    // for (float x = m_sphWallBox.min.x + spacing; x <= m_sphWallBox.max.x; x += spacing)
    // {
    //     Point *p = m_wallBuffer.addPointReuse();
    //     p->pos = glm::vec3(x, m_sphWallBox.min.y, m_sphWallBox.min.z);
    //     p->type = 1;
    //     p = m_wallBuffer.addPointReuse();
    //     p->pos = glm::vec3(x, m_sphWallBox.min.y, m_sphWallBox.max.z);
    //     p->type = 1;
    //     p = m_wallBuffer.addPointReuse();
    //     p->pos = glm::vec3(x, m_sphWallBox.max.y, m_sphWallBox.min.z);
    //     p->type = 1;
    //     p = m_wallBuffer.addPointReuse();
    //     p->pos = glm::vec3(x, m_sphWallBox.max.y, m_sphWallBox.max.z);
    //     p->type = 1;
    // }
    // for (float z = m_sphWallBox.min.z + spacing; z <= m_sphWallBox.max.z; z += spacing)
    // {
    //     Point *p = m_wallBuffer.addPointReuse();
    //     p->pos = glm::vec3(m_sphWallBox.min.x, m_sphWallBox.min.y, z);
    //     p->type = 1;
    //     p = m_wallBuffer.addPointReuse();
    //     p->pos = glm::vec3(m_sphWallBox.min.x, m_sphWallBox.max.y, z);
    //     p->type = 1;
    //     p = m_wallBuffer.addPointReuse();
    //     p->pos = glm::vec3(m_sphWallBox.max.x, m_sphWallBox.min.y, z);
    //     p->type = 1;
    //     p = m_wallBuffer.addPointReuse();
    //     p->pos = glm::vec3(m_sphWallBox.max.x, m_sphWallBox.max.y, z);
    //     p->type = 1;
    // }
    std::cout << cnt << std::endl;
}

void FluidSystem::_updatePosAndVel(int L, int R)
{
    for (int i = L; i < R; i++)
    {
        Point *pi = m_pointBuffer.get(i);
        pi->force += pi->pressure_force;
        const float invMass = 1.0 / m_pointMass;

        // Leapfrog integration
        glm::vec3 vnext = pi->velocity + pi->force * invMass * timeStep; // v(t+1) = v(t-1) + a(t) dt
        pi->velocity = vnext;
        pi->pos += vnext * timeStep / m_unitScale; // p(t+1) = p(t) + v(t+1) dt
        std::lock_guard<std::mutex> lock(mtx);     // 锁定互斥锁
        // 弹入位置数据
        posData[3 * i] = pi->pos.x;
        posData[3 * i + 1] = pi->pos.y;
        posData[3 * i + 2] = pi->pos.z;
    }
}

void FluidSystem::buildNeighborList()
{
    float h2 = m_smoothRadius * m_smoothRadius;  // h^2
    m_neighborTable.reset(m_pointBuffer.size()); // 重置邻接表
    for (unsigned int i = 0; i < m_pointBuffer.size(); i++)
    {
        _computeNeighbor(i);
    }
}

float FluidSystem::_computeNeighbor(int i)
{
    float h2 = m_smoothRadius * m_smoothRadius;
    Point *pi = m_pointBuffer.get(i);
    float sum = 0.0;
    m_neighborTable.point_prepare(i);
    int gridCell[27];
    m_gridContainer.findCells(pi->pos, m_smoothRadius / m_unitScale, gridCell);
    for (int cell = 0; cell < 27; cell++)
    {
        if (gridCell[cell] == -1)
            continue;
        int pndx = m_gridContainer.getGridData(gridCell[cell]);
        while (pndx != -1)
        {
            Point *pj = m_pointBuffer.get(pndx);
            if (pj == pi)
                sum += pow(h2, 3.0);
            else
            {
                glm::vec3 pi_pj = (pi->pos - pj->pos) * m_unitScale;
                float r2 = pi_pj.x * pi_pj.x + pi_pj.y * pi_pj.y + pi_pj.z * pi_pj.z;
                if (h2 > r2)
                {
                    float h2_r2 = h2 - r2;
                    sum += pow(h2_r2, 3.0);
                    if (!m_neighborTable.point_add_neighbor(pndx, glm::sqrt(r2)))
                    {
                        m_neighborTable.point_commit();
                        return sum;
                    }
                }
            }
            pndx = pj->next;
        }
    }
    m_neighborTable.point_commit();
    return sum;
}
glm::vec3 FluidSystem::_boundaryForce(Point *p)
{
    glm::vec3 accel = glm::vec3(0);

    float diff;
    // 边界情况
    //  Z方向边界
    diff = 0.08f * m_unitScale - (p->pos.z - m_sphWallBox.min.z) * m_unitScale;
    if (diff > 0.0f)
    {
        glm::vec3 norm(0, 0, 1.0);
        float adj = m_boundaryStiffness * diff - m_boundaryDampening * glm::dot(norm, p->velocity);
        accel.z += adj * norm.z;
    }

    diff = 0.08f * m_unitScale - (m_sphWallBox.max.z - p->pos.z) * m_unitScale;
    if (diff > 0.0f)
    {
        glm::vec3 norm(0, 0, -1.0);
        float adj = m_boundaryStiffness * diff - m_boundaryDampening * glm::dot(norm, p->velocity);
        accel.z += adj * norm.z;
    }

    // X方向边界
    diff = 0.08f * m_unitScale - (p->pos.x - m_sphWallBox.min.x) * m_unitScale;
    if (diff > 0.0f)
    {
        glm::vec3 norm(1.0, 0, 0);
        float adj = m_boundaryStiffness * diff - m_boundaryDampening * glm::dot(norm, p->velocity);
        accel.x += adj * norm.x;
    }

    diff = 0.08f * m_unitScale - (m_sphWallBox.max.x - p->pos.x) * m_unitScale;
    if (diff > 0.0f)
    {
        glm::vec3 norm(-1.0, 0, 0);
        float adj = m_boundaryStiffness * diff - m_boundaryDampening * glm::dot(norm, p->velocity);
        accel.x += adj * norm.x;
    }

    // Y方向边界
    diff = 0.08f * m_unitScale - (p->pos.y - m_sphWallBox.min.y) * m_unitScale;
    if (diff > 0.0f)
    {
        glm::vec3 norm(0, 1.0, 0);
        float adj = m_boundaryStiffness * diff - m_boundaryDampening * glm::dot(norm, p->velocity);
        accel.y += adj * norm.y;
    }
    diff = 0.08f * m_unitScale - (m_sphWallBox.max.y - p->pos.y) * m_unitScale;
    if (diff > 0.0f)
    {
        glm::vec3 norm(0, -1.0, 0);
        float adj = m_boundaryStiffness * diff - m_boundaryDampening * glm::dot(norm, p->velocity);
        accel.y += adj * norm.y;
    }

    return accel;
}
void FluidSystem::computeForce()
{
    vector<thread> threads;
    int cnt = ceil(1.0f * m_pointBuffer.size() / threadCnt);
    for (int i = 0; i < threadCnt; i++)
    {
        int l = i * cnt, r = min(l + cnt, (int)m_pointBuffer.size());
        threads.push_back(thread(_computerForce, this, l, r));
    }
    for (auto &thread : threads)
    {
        thread.join();
    }
}

float *FluidSystem::getSufPosNorBuf()
{
    m_vrt_nrms.clear();
    for (int i = 0; i < m_vrts.size(); i++)
    {
        glm::vec3 posTem = m_vrts[i];
        glm::vec3 nrmTem = m_nrms[i];
        m_vrt_nrms.push_back(posTem);
        m_vrt_nrms.push_back(nrmTem);
    }
    return &(m_vrt_nrms[0].x);
}

unsigned int *FluidSystem::getSufTriIdxBuf()
{
    m_tri_idx.clear();
    for (int i = 0; i < m_face.size(); i++)
    {
        for (int j = 0; j < m_face[i].vert_idx.size(); j++)
        {
            m_tri_idx.push_back(m_face[i].vert_idx[j]);
        }
    }
    return m_tri_idx.data();
}

void FluidSystem::_computePredictedDensityAndPressure(int L, int R)
{
    for (int i = L; i < R; i++)
    {
        float h2 = m_smoothRadius * m_smoothRadius;
        mtx.lock();
        Point *pi = m_pointBuffer.get(i);
        int neighborCounts = m_neighborTable.getNeighborCounts(i);
        mtx.unlock();
        pi->predicted_density = pi->kernel_self * m_pointMass;
        // 预测密度计算
        for (int j = 0; j < neighborCounts; j++)
        {
            unsigned int neighborIndex;
            float r;
            mtx.lock();
            m_neighborTable.getNeighborInfo(i, j, neighborIndex, r);
            Point *pj = m_pointBuffer.get(neighborIndex);
            mtx.unlock();
            // 需要用预测的位置去重新计算距离和核值
            glm::vec3 pi_pj = (pi->predicted_position - pj->predicted_position) * m_unitScale;
            float r2 = pi_pj.x * pi_pj.x + pi_pj.y * pi_pj.y + pi_pj.z * pi_pj.z;
            if (h2 > r2)
            {
                float h2_r2 = h2 - r2;
                pi->predicted_density += m_kernelPoly6 * pow(h2_r2, 3.0) * m_pointMass;
            }
        }
        auto sss = pi->predicted_density;
        // 计算密度误差，仅考虑正向误差
        pi->density_error = glm::max(pi->predicted_density - m_restDensity, 0.0f);

        // 更新压力,只矫正正压力，densityErrorFactor被预先计算并用作常数
        pi->pressure += glm::max(pi->density_error * m_densityErrorFactor, 0.0f);

        max_predicted_density = glm::max(max_predicted_density, pi->predicted_density);
    }
}

void FluidSystem::_computeCorrectivePressureForce(int L, int R)
{
    for (int i = L; i < R; i++)
    {
        float h2 = m_smoothRadius * m_smoothRadius;
        mtx.lock();
        Point *pi = m_pointBuffer.get(i);
        int neighborCounts = m_neighborTable.getNeighborCounts(i);
        mtx.unlock();
        pi->pressure_force = glm::vec3(0.0f);

        float densSq = m_restDensity * m_restDensity;
        float pi_pres = pi->pressure;

        // 更新压力
        for (int j = 0; j < neighborCounts; j++)
        {
            unsigned int neighborIndex;
            float r;
            mtx.lock();
            m_neighborTable.getNeighborInfo(i, j, neighborIndex, r);
            Point *pj = m_pointBuffer.get(neighborIndex);
            mtx.unlock();
            // 需要用预测的位置去重新计算距离和核值
            glm::vec3 pi_pj = (pi->pos - pj->pos) * m_unitScale;
            float r2 = pi_pj.x * pi_pj.x + pi_pj.y * pi_pj.y + pi_pj.z * pi_pj.z;
            if (h2 > r2)
            {
                r = pow(r2, 0.5);
                float h = m_smoothRadius;
                float pj_pres = pj->pressure;
                glm::vec3 gradientKer = pi_pj * m_kernelSpiky / r * (h - r) * (h - r);
                float grad = pi_pres / densSq + pj_pres / (m_restDensity * m_restDensity);
                pi->pressure_force -= m_pointMass * m_pointMass * grad * gradientKer;
            }
        }
    }
}

void FluidSystem::_collisionHandling(glm::vec3 &pos, glm::vec3 &vel)
{
}

void FluidSystem::predictionCorrectionStep()
{
    density_error_too_large = true;
    int iteration = 0;
    while ((iteration < minLoops) || ((density_error_too_large) && (iteration < maxLoops)))
    {
        vector<thread> threads;
        int cnt = ceil(1.0f * m_pointBuffer.size() / threadCnt);
        for (int i = 0; i < threadCnt; i++)
        {
            int l = i * cnt, r = min(l + cnt, (int)m_pointBuffer.size());
            threads.push_back(thread(_computePosVel, this, l, r));
        }
        for (auto &thread : threads)
        {
            thread.join();
        }

        // 循环终止条件(所有粒子的最大预测密度)
        max_predicted_density = 0.0;

        threads.clear();
        for (int i = 0; i < threadCnt; i++)
        {
            int l = i * cnt, r = min(l + cnt, (int)m_pointBuffer.size());
            threads.push_back(thread(_computePredictedDensityAndPressure, this, l, r));
        }
        for (auto &thread : threads)
        {
            thread.join();
        }

        // 循环终止条件
        float densityErrorInPercent = glm::max(0.1f * max_predicted_density - 100.0f, 0.0f);
        float maxDensityErrorAllowed = 1.0f;

        // 如果密度误差小于终止条件，则终止循环(小于密度误差阈值)
        if (densityErrorInPercent < maxDensityErrorAllowed)
            density_error_too_large = false;

        threads.clear();
        for (int i = 0; i < threadCnt; i++)
        {
            int l = i * cnt, r = min(l + cnt, (int)m_pointBuffer.size());
            threads.push_back(thread(_computeCorrectivePressureForce, this, l, r));
        }
        for (auto &thread : threads)
        {
            thread.join();
        }
        iteration++;
    }
}

void FluidSystem::updatePosAndVel()
{
    vector<thread> threads;
    int cnt = ceil(1.0f * m_pointBuffer.size() / 1);

    for (int i = 0; i < 1; i++)
    {
        int l = i * cnt, r = min(l + cnt, (int)m_pointBuffer.size());
        threads.push_back(thread(_updatePosAndVel, this, l, r));
    }
    for (auto &thread : threads)
    {
        thread.join();
    }
}
void FluidSystem::_init(unsigned int maxPointCounts, const fBox3 &wallBox, const fBox3 &initFluidBox, const glm::vec3 &gravity)
{
    m_pointBuffer.reset(maxPointCounts);
    m_wallBuffer.reset(maxPointCounts);
    m_sphWallBox = wallBox;
    m_gravityDir = gravity;

    // 根据粒子间距计算粒子质量
    m_pointMass = m_restDensity * pow(m_pointDistance, 3.0);
    // 设定流体块
    _addFluidVolume(initFluidBox, m_pointDistance / m_unitScale);
    m_mcMesh = rxMCMesh();
    m_gridContainer.init(wallBox, m_unitScale, m_smoothRadius, 1.0, m_rexSize); // 设置网格尺寸(2r)
// 初始化标量场
#ifdef SUF
    m_field = new float[(m_rexSize[0] + 1) * (m_rexSize[1] + 1) * (m_rexSize[2] + 1)]();
#endif
    posData = std::vector<float>(3 * m_pointBuffer.size(), 0);
    m_gridContainer.insertParticles(&m_pointBuffer); // 每帧刷新粒子位置
    m_neighborTable.reset(m_pointBuffer.size());     // 重置邻接表
    // 计算W的梯度
    _computeGradWValues();
    // 计算因子
    _computeDensityErrorFactor();
}
double FluidSystem::GetImplicit(double x, double y, double z)
{
    return CalColorField(x, y, z);
}
void FluidSystem::CalImplicitField(int n[3], glm::vec3 minp, glm::vec3 d, float *hF)
{
    int slice0 = n[0] + 1;
    int slice1 = slice0 * (n[1] + 1);

    auto myfun = [this](int L, int R, int n[], int slice1, int slice0, glm::vec3 minp, glm::vec3 d, float *hF) -> void
    {
        for (int k = L; k < R; k++)
        {
            for (int j = 0; j <= n[1]; ++j)
            {
                for (int i = 0; i <= n[0]; ++i)
                {
                    int idx = k * slice1 + j * slice0 + i;
                    glm::vec3 pos = minp + glm::vec3(i, j, k) * d;
                    hF[idx] = GetImplicit(pos[0], pos[1], pos[2]);
                }
            }
        }
    };
    vector<thread> threads;
    int cnt = ceil(1.0f * n[2] / threadCnt);
    for (int i = 0; i < threadCnt; i++)
    {
        int l = i * cnt, r = min(l + cnt, n[2]);
        threads.push_back(thread(myfun, l, r, n, slice1, slice0, minp, d, hF));
    }
    for (auto &thread : threads)
    {
        thread.join();
    }
}
void FluidSystem::CalImplicitFieldDevice(int n[3], glm::vec3 minp, glm::vec3 d, float *dF)
{
    float *hF = new float[(n[0] + 1) * (n[1] + 1) * (n[2] + 1)]();

    CalImplicitField(n, minp, d, hF);
    CuCopyArrayToDevice(dF, hF, 0, (n[0] + 1) * (n[1] + 1) * (n[2] + 1) * sizeof(float));
    delete[] hF;
}
double FluidSystem::CalColorField(double x, double y, double z)
{
    // MRK:CalColorField
    float c = 0.0;
    glm::vec3 pos(x, y, z);

    if (pos[0] < m_sphWallBox.min[0] - eps)
        return c;
    if (pos[0] > m_sphWallBox.max[0] + eps)
        return c;
    if (pos[1] < m_sphWallBox.min[1] - eps)
        return c;
    if (pos[1] > m_sphWallBox.max[1] + eps)
        return c;
    if (pos[2] < m_sphWallBox.min[2] - eps)
        return c;
    if (pos[2] > m_sphWallBox.max[2] + eps)
        return c;

    float h = m_smoothRadius;

    int cell[27];
    m_gridContainer.findCells(pos, h / m_unitScale, cell);

    // 近傍粒子(各向同性)
    for (int i = 0; i < 27; i++)
    {
        if (cell[i] < 0)
            continue;
        int pndx = m_gridContainer.getGridData(cell[i]);
        while (pndx != -1)
        {
            Point *p = m_pointBuffer.get(pndx);
            float r = glm::distance(pos, p->pos) * m_unitScale;
            float q = h * h - r * r;
            if (q > 0)
            {
                c += m_pointMass * m_kernelPoly6 * q * q * q;
            }
            pndx = p->next;
        }
    }
    return c;
}

// 获取三角片面缓存
float *FluidSystem::getPolygonBuf()
{
    m_polyBuf.clear();
    for (int i = 0; i < m_face.size(); i++)
    {
        for (int j = 0; j < m_face[i].vert_idx.size(); j++)
        {
            glm::vec3 posTem = m_vrts[m_face[i].vert_idx[j]];
            m_polyBuf.push_back(posTem.x);
            m_polyBuf.push_back(posTem.y);
            m_polyBuf.push_back(posTem.z);
        }
    }
    return &m_polyBuf[0];
}

// 获取顶点个数
int FluidSystem::getPolyNum()
{
    int tem = 0;
    return m_face.size() * 3;
}

// 保存所有网格隐函数值
void FluidSystem::CuCopyArrayToDevice(float *device, const float *host, int offset, int size)
{
    memcpy(device + offset, host, size);
}

void FluidSystem::_computeGradWValues()
{
    float h2 = m_smoothRadius * m_smoothRadius; // h^2
    const int numP = m_pointBuffer.size();
    for (int i = 0; i < numP; i++)
    {
        Point *pi = m_pointBuffer.get(i);
        pi->sum_grad_w = glm::vec3(0.0f);
        pi->sum_grad_w_dot = 0.0f;
    }
    for (int i = 0; i < numP; i++)
    {
        Point *pi = m_pointBuffer.get(i);
        pi->kernel_self = _computeNeighbor(i);
        m_neighborTable.point_commit();
        int neighborCounts = m_neighborTable.getNeighborCounts(i);
        // 预测密度计算
        for (int j = 0; j < neighborCounts; j++)
        {
            unsigned int neighborIndex;
            float r;
            m_neighborTable.getNeighborInfo(i, j, neighborIndex, r);
            Point *pj = m_pointBuffer.get(neighborIndex);
            // 需要用预测的位置去重新计算距离和核值
            glm::vec3 pi_pj = (pi->pos - pj->pos) * m_unitScale;
            float r2 = pi_pj.x * pi_pj.x + pi_pj.y * pi_pj.y + pi_pj.z * pi_pj.z;
            if (h2 > r2)
            {
                float h2_r2 = h2 - r2;
                r = pow(r2, 0.5f);
                float h = m_smoothRadius;

                glm::vec3 gradVec = (pi->pos - pj->pos) * m_kernelSpiky / r * (h - r) * (h - r);
                pi->sum_grad_w += gradVec;
                pj->sum_grad_w -= gradVec;

                pi->sum_grad_w_dot += glm::dot(gradVec, gradVec);
                pj->sum_grad_w_dot += glm::dot(-1.0f * gradVec, -1.0f * gradVec);
            }
        }
    }
}
void FluidSystem::_computeDensityErrorFactor()
{
    float h2 = m_smoothRadius * m_smoothRadius;
    int maxNeighborIndex = 0;
    int maxNeighs = 0;
    const int numParticles = m_pointBuffer.size();
    for (int i = 0; i < numParticles; i++)
    {
        Point *pi = m_pointBuffer.get(i);
        int neighborCounts = m_neighborTable.getNeighborCounts(i);
        int numNeighbors = 0;
        // 预测密度计算
        for (int j = 0; j < neighborCounts; j++)
        {
            unsigned int neighborIndex;
            float r;
            m_neighborTable.getNeighborInfo(i, j, neighborIndex, r);
            Point *pj = m_pointBuffer.get(neighborIndex);
            // 需要用预测的位置去重新计算距离和核值
            glm::vec3 pi_pj = (pi->pos - pj->pos) * m_unitScale;
            float r2 = pi_pj.x * pi_pj.x + pi_pj.y * pi_pj.y + pi_pj.z * pi_pj.z;
            if (h2 > r2)
            {
                numNeighbors++;
            }
        }
        // 获取邻居最多的粒子，和邻居个数
        if (numNeighbors > maxNeighs)
        {
            maxNeighs = numNeighbors;
            maxNeighborIndex = i;
        }
    }
    // 获取邻居最多的粒子
    Point *pmax = m_pointBuffer.get(maxNeighborIndex);

    // 计算新的压力根据密度误差
    float restVol = m_pointMass / m_restDensity;
    float preFactor = 2.0 * restVol * restVol * timeStep * timeStep;
    float gradWTerm = glm::dot(pmax->sum_grad_w * (-1.0f), pmax->sum_grad_w) - pmax->sum_grad_w_dot;
    float divisor = preFactor * gradWTerm;
    m_densityErrorFactor = -1.0 / divisor;
    const float factor = timeStep / 0.0001f;
    float densityErrorFactorParameter = 0.05 * factor * factor;
    m_densityErrorFactor *= 1.0 * densityErrorFactorParameter;
}
void FluidSystem::_computerForce(int L, int R)
{
    for (int i = L; i < R; i++)
    {
        mtx.lock(); // 锁定互斥锁
        Point *pi = m_pointBuffer.get(i);
        pi->force = glm::vec3(0.0);

        // 邻居粒子装载
        pi->kernel_self = _computeNeighbor(i);
        m_neighborTable.point_commit();

        // 外力计算
        int neighborCounts = m_neighborTable.getNeighborCounts(i);
        mtx.unlock();
        const float restVolume = m_pointMass / m_restDensity;
        for (unsigned int j = 0; j < neighborCounts; j++)
        {
            unsigned int neighborIndex;
            float r;
            mtx.lock(); // 锁定互斥锁
            m_neighborTable.getNeighborInfo(i, j, neighborIndex, r);
            Point *pj = m_pointBuffer.get(neighborIndex);
            mtx.unlock();
            float h_r = m_smoothRadius - r;

            // F_Viscosity
            // m_kernelViscosity = 45.0f/(3.141592f * h^6);
            float vterm = m_kernelViscosity * m_viscosity * h_r * restVolume * restVolume;
            pi->force -= (pi->velocity - pj->velocity) * vterm;
        }
        // F_gravity
        pi->force += m_gravityDir * m_pointMass;

        // F_boundary
        pi->force += _boundaryForce(pi) * m_pointMass;

        // 初始化矫正因子
        pi->pressure = 0.0f;
        pi->pressure_force = glm::vec3(0.0);
    }
}
void FluidSystem::_computePosVel(int L, int R)
{
    for (int i = L; i < R; i++)
    {
        mtx.lock();
        Point *p = m_pointBuffer.get(i);
        mtx.unlock();
        // 计算预测速度和位置
        p->predicted_velocity = p->velocity + (p->force + p->pressure_force) * timeStep / m_pointMass;
        p->predicted_position = p->pos + p->predicted_velocity * timeStep;

        // 碰撞处理
        _collisionHandling(p->predicted_position, p->predicted_velocity);

        // 初始化预测密度
        p->predicted_density = 0.0;
    }
}
void FluidSystem::tick()
{

    m_gridContainer.insertParticles(&m_pointBuffer); // 每帧刷新粒子位置
    m_neighborTable.reset(m_pointBuffer.size());     // 重置邻接表
    // 计算除压力的所有力
    // std::cout << 1 << " " << posData.size() << std::endl;
    computeForce();
    // std::cout << 1 << " " << posData.size() << std::endl;
    // 预测矫正压强
    predictionCorrectionStep();
    // std::cout << 2 << " " << posData.size() << std::endl;
    // 更新粒子数据
    updatePosAndVel();
    // std::cout << 3 << " " << posData.size() << std::endl;
#ifdef SUF
    tickCnt++;
    if (tickCnt % 50 == 0)
    {
        glm::vec3 tem = m_sphWallBox.min;

        CalImplicitFieldDevice(m_rexSize, tem, glm::vec3(0.125 / m_gridContainer.getDelta()), m_field);
        clearSuf(); // 清空表面数据
        m_mcMesh.CreateMeshV(m_field, tem, 0.125 / m_gridContainer.getDelta(), m_rexSize, m_thre, m_vrts, m_nrms, m_face);
    }

#endif
}
