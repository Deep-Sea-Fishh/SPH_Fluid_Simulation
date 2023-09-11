#include "fluidSystem.h"
#include <thread>
#include <cstring>
#include <iostream>
const float eps = 1e-6;
int tickCnt = 0;
std::mutex posDataMtx;
std::mutex mtx;
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

    m_boundaryStiffness = 800.0f;
    m_boundaryDampening = 150.0f;
    m_speedLimiting = 0;

    m_thre = 100;

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
        posDataMtx.lock();
        // 弹入位置数据
        posData[3 * i] = pi->pos.x;
        posData[3 * i + 1] = pi->pos.y;
        posData[3 * i + 2] = pi->pos.z;
        posDataMtx.unlock();
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
    m_gridContainer.init(wallBox, m_unitScale, m_smoothRadius, 0.1f, m_rexSize); // 设置网格尺寸(2r)
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

    setAnisotropic0();
    m_hEigen.resize(3 * m_pointBuffer.size());
    m_hRMatrix.resize(9 * m_pointBuffer.size());
    m_hG.resize(9 * m_pointBuffer.size());
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
    if (x < m_sphWallBox.min.x)
        return c;
    if (x > m_sphWallBox.max.x)
        return c;
    if (y < m_sphWallBox.min.y)
        return c;
    if (y > m_sphWallBox.max.y)
        return c;
    if (z < m_sphWallBox.min.z)
        return c;
    if (z > m_sphWallBox.max.z)
        return c;
    glm::vec3 pos(x, y, z);
    float h = m_smoothRadius;

    int cell[27];
    m_gridContainer.findCells(pos, h / m_unitScale, cell);

    if (!isAnisotropic)
    {
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
    }
    else
    {
        // 近傍粒子(各向异性)
        for (int i = 0; i < 27; i++)
        {
            if (cell[i] < 0)
                continue;
            int pndx = m_gridContainer.getGridData(cell[i]);
            while (pndx != -1)
            {
                glm::mat3 G(0.0);
                for (int j = 0; j < 3; ++j)
                {
                    for (int k = 0; k < 3; ++k)
                    {
                        G[k][j] = m_hG[pndx * 9 + 3 * j + k];
                    }
                }
                Point *p = m_pointBuffer.get(pndx);
                glm::vec3 rij = pos - p->pos;
                rij = G * rij; // 形变后的椭球体距离
                float r = glm::length(rij) * m_unitScale;
                float q = h * h - r * r;
                if (q > 0)
                {
                    float detG = glm::determinant(G); // 计算行列式
                    c += detG * m_pointMass * m_kernelPoly6 * q * q * q;
                }
                pndx = p->next;
            }
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
        if (isAnisotropic)
            CalAnisotropicKernel();
        glm::vec3 tem = m_sphWallBox.min - glm::vec3(0.1);

        CalImplicitFieldDevice(m_rexSize, tem, glm::vec3(0.125 / m_gridContainer.getDelta()), m_field);
        clearSuf(); // 清空表面数据
        m_mcMesh.CreateMeshV(m_field, tem, 0.125 / m_gridContainer.getDelta(), m_rexSize, m_thre, m_vrts, m_nrms, m_face);
        if (isAnisotropic)
        {
            // 返回最初点位置
            for (int i = 0; i < m_hOldPos.size() / 3; ++i)
            {
                Point *p = m_pointBuffer.get(i);
                p->pos = glm::vec3(m_hOldPos[3 * i], m_hOldPos[3 * i + 1], m_hOldPos[3 * i + 2]);
            }
        }
    }
#endif
}
void FluidSystem::RxSVDecomp3(float w[3], float u[9], float v[9], float eps)
{

    bool flag;
    int i, its, j, jj, k, l, nm;
    float anorm, c, f, g, h, s, scale, x, y, z;
    float rv1[3];
    g = scale = anorm = 0.0;
    for (i = 0; i < 3; ++i)
    {
        l = i + 2;
        rv1[i] = scale * g;
        g = s = scale = 0.0;
        for (k = i; k < 3; ++k)
            scale += abs(u[k * 3 + i]);
        if (scale != 0.0)
        {
            for (k = i; k < 3; ++k)
            {
                u[k * 3 + i] /= scale;
                s += u[k * 3 + i] * u[k * 3 + i];
            }
            f = u[i * 3 + i];
            g = -RX_SIGN2(sqrt(s), f);
            h = f * g - s;
            u[i * 3 + i] = f - g;
            for (j = l - 1; j < 3; ++j)
            {
                for (s = 0.0, k = i; k < 3; ++k)
                    s += u[k * 3 + i] * u[k * 3 + j];
                f = s / h;
                for (k = i; k < 3; ++k)
                    u[k * 3 + j] += f * u[k * 3 + i];
            }
            for (k = i; k < 3; ++k)
                u[k * 3 + i] *= scale;
        }

        w[i] = scale * g;
        g = s = scale = 0.0;
        if (i + 1 <= 3 && i + 1 != 3)
        {
            for (k = l - 1; k < 3; ++k)
                scale += abs(u[i * 3 + k]);
            if (scale != 0.0)
            {
                for (k = l - 1; k < 3; ++k)
                {
                    u[i * 3 + k] /= scale;
                    s += u[i * 3 + k] * u[i * 3 + k];
                }
                f = u[i * 3 + l - 1];
                g = -RX_SIGN2(sqrt(s), f);
                h = f * g - s;
                u[i * 3 + l - 1] = f - g;
                for (k = l - 1; k < 3; ++k)
                    rv1[k] = u[i * 3 + k] / h;
                for (j = l - 1; j < 3; ++j)
                {
                    for (s = 0.0, k = l - 1; k < 3; ++k)
                        s += u[j * 3 + k] * u[i * 3 + k];
                    for (k = l - 1; k < 3; ++k)
                        u[j * 3 + k] += s * rv1[k];
                }
                for (k = l - 1; k < 3; ++k)
                    u[i * 3 + k] *= scale;
            }
        }
        anorm = RX_MAX(anorm, (abs(w[i]) + abs(rv1[i])));
    }

    for (i = 2; i >= 0; --i)
    {
        if (i < 2)
        {
            if (g != 0.0)
            {
                for (j = l; j < 3; ++j)
                {
                    v[j * 3 + i] = (u[i * 3 + j] / u[i * 3 + l]) / g;
                }
                for (j = l; j < 3; ++j)
                {
                    for (s = 0.0, k = l; k < 3; ++k)
                        s += u[i * 3 + k] * v[k * 3 + j];
                    for (k = l; k < 3; ++k)
                        v[k * 3 + j] += s * v[k * 3 + i];
                }
            }
            for (j = l; j < 3; ++j)
                v[i * 3 + j] = v[j * 3 + i] = 0.0;
        }
        v[i * 3 + i] = 1.0;
        g = rv1[i];
        l = i;
    }
    for (i = 2; i >= 0; --i)
    {
        l = i + 1;
        g = w[i];
        for (j = l; j < 3; ++j)
            u[i * 3 + j] = 0.0;
        if (g != 0.0)
        {
            g = 1.0 / g;
            for (j = l; j < 3; ++j)
            {
                for (s = 0.0, k = l; k < 3; ++k)
                    s += u[k * 3 + i] * u[k * 3 + j];
                f = (s / u[i * 3 + i]) * g;
                for (k = i; k < 3; ++k)
                    u[k * 3 + j] += f * u[k * 3 + i];
            }
            for (j = i; j < 3; ++j)
                u[j * 3 + i] *= g;
        }
        else
        {
            for (j = i; j < 3; ++j)
                u[j * 3 + i] = 0.0;
        }
        ++u[i * 3 + i];
    }
    for (k = 2; k >= 0; --k)
    {
        for (its = 0; its < 60; ++its)
        {
            flag = true;
            for (l = k; l >= 0; --l)
            {
                nm = l - 1;
                if (l == 0 || abs(rv1[l]) <= eps * anorm)
                {
                    flag = false;
                    break;
                }
                if (abs(w[nm]) <= eps * anorm)
                    break;
            }
            if (flag)
            {
                c = 0.0;
                s = 1.0;
                for (i = l; i < k + 1; ++i)
                {
                    f = s * rv1[i];
                    rv1[i] = c * rv1[i];
                    if (abs(f) <= eps * anorm)
                        break;
                    g = w[i];
                    h = RxPythag(f, g);
                    w[i] = h;
                    h = 1.0 / h;
                    c = g * h;
                    s = -f * h;
                    for (j = 0; j < 3; ++j)
                    {
                        y = u[j * 3 + nm];
                        z = u[j * 3 + i];
                        u[j * 3 + nm] = y * c + z * s;
                        u[j * 3 + i] = z * c - y * s;
                    }
                }
            }
            z = w[k];
            if (l == k)
            {
                if (z < 0.0)
                {
                    w[k] = -z;
                    for (j = 0; j < 3; ++j)
                        v[j * 3 + k] = -v[j * 3 + k];
                }
                break;
            }
            if (its == 59)
            {
                printf("no convergence in 60 svdcmp iterations");
                return;
            }
            x = w[l];
            nm = k - 1;
            y = w[nm];
            g = rv1[nm];
            h = rv1[k];
            f = ((y - z) * (y + z) + (g - h) * (g + h)) / (2.0 * h * y);
            g = RxPythag(f, 1.0f);
            f = ((x - z) * (x + z) + h * ((y / (f + RX_SIGN2(g, f))) - h)) / x;
            c = s = 1.0;
            for (j = l; j <= nm; ++j)
            {
                i = j + 1;
                g = rv1[i];
                y = w[i];
                h = s * g;
                g = c * g;
                z = RxPythag(f, h);
                rv1[j] = z;
                c = f / z;
                s = h / z;
                f = x * c + g * s;
                g = g * c - x * s;
                h = y * s;
                y *= c;
                for (jj = 0; jj < 3; ++jj)
                {
                    x = v[jj * 3 + j];
                    z = v[jj * 3 + i];
                    v[jj * 3 + j] = x * c + z * s;
                    v[jj * 3 + i] = z * c - x * s;
                }
                z = RxPythag(f, h);
                w[j] = z;
                if (z)
                {
                    z = 1.0 / z;
                    c = f * z;
                    s = h * z;
                }
                f = c * g + s * y;
                x = c * y - s * g;
                for (jj = 0; jj < 3; ++jj)
                {
                    y = u[jj * 3 + j];
                    z = u[jj * 3 + i];
                    u[jj * 3 + j] = y * c + z * s;
                    u[jj * 3 + i] = z * c - y * s;
                }
            }
            rv1[l] = 0.0;
            rv1[k] = f;
            w[k] = x;
        }
    }
    // reorder
    int inc = 1;
    float sw;
    float su[3], sv[3];

    do
    {
        inc *= 3;
        inc++;
    } while (inc <= 3);

    do
    {
        inc /= 3;
        for (i = inc; i < 3; ++i)
        {
            sw = w[i];
            for (k = 0; k < 3; ++k)
                su[k] = u[k * 3 + i];
            for (k = 0; k < 3; ++k)
                sv[k] = v[k * 3 + i];
            j = i;
            while (w[j - inc] < sw)
            {
                w[j] = w[j - inc];
                for (k = 0; k < 3; ++k)
                    u[k * 3 + j] = u[k * 3 + j - inc];
                for (k = 0; k < 3; ++k)
                    v[k * 3 + j] = v[k * 3 + j - inc];
                j -= inc;
                if (j < inc)
                    break;
            }
            w[j] = sw;
            for (k = 0; k < 3; ++k)
                u[k * 3 + j] = su[k];
            for (k = 0; k < 3; ++k)
                v[k * 3 + j] = sv[k];
        }
    } while (inc > 1);
    for (k = 0; k < 3; ++k)
    {
        s = 0;
        for (i = 0; i < 3; ++i)
            if (u[i * 3 + k] < 0.)
                s++;
        for (j = 0; j < 3; ++j)
            if (v[j * 3 + k] < 0.)
                s++;
        if (s > 3)
        {
            for (i = 0; i < 3; ++i)
                u[i * 3 + k] = -u[i * 3 + k];
            for (j = 0; j < 3; ++j)
                v[j * 3 + k] = -v[j * 3 + k];
        }
    }
}
void FluidSystem::CalAnisotropicKernel()
{
    if (m_pointBuffer.size() == 0)
        return;

    float h0 = m_smoothRadius;
    float h = 2.0 * h0;

    float lambda = 0.9;

    // 计算更新位置
    m_hPosW.resize(m_pointBuffer.size() * 3, 0.0);
    for (unsigned int i = 0; i < m_pointBuffer.size(); ++i)
    {
        glm::vec3 pos0;
        pos0[0] = m_pointBuffer.get(i)->pos.x;
        pos0[1] = m_pointBuffer.get(i)->pos.y;
        pos0[2] = m_pointBuffer.get(i)->pos.z;

        // 相邻的粒子
        glm::vec3 posw(0.0);
        float sumw = 0.0f;

        int cell[27 * 27]; // 2*smoothRadius的领域网格
        m_gridContainer.findTwoCells(pos0, h / m_unitScale, cell);
        // 计算x_mean
        for (int j = 0; j < 27 * 27; j++)
        {
            if (cell[j] == -1)
                continue;
            int pndx = m_gridContainer.getGridData(cell[j]);
            while (pndx != -1)
            {
                glm::vec3 pos1;
                pos1[0] = m_pointBuffer.get(pndx)->pos.x;
                pos1[1] = m_pointBuffer.get(pndx)->pos.y;
                pos1[2] = m_pointBuffer.get(pndx)->pos.z;

                float r = glm::length(pos1 - pos0) * m_unitScale;

                if (r < h)
                {
                    float q = 1 - r / h;
                    float wij = q * q * q;
                    posw += pos1 * wij;
                    sumw += wij;
                }
                pndx = m_pointBuffer.get(pndx)->next;
            }
        }

        int DIM = 3;
        m_hPosW[DIM * i + 0] = posw[0] / sumw;
        m_hPosW[DIM * i + 1] = posw[1] / sumw;
        m_hPosW[DIM * i + 2] = posw[2] / sumw;

        // m_hPosW[DIM * i + 0] = (1 - lambda) * m_pointBuffer.get(i)->pos.x + lambda * m_hPosW[DIM * i + 0];
        // m_hPosW[DIM * i + 1] = (1 - lambda) * m_pointBuffer.get(i)->pos.y + lambda * m_hPosW[DIM * i + 1];
        // m_hPosW[DIM * i + 2] = (1 - lambda) * m_pointBuffer.get(i)->pos.z + lambda * m_hPosW[DIM * i + 2];
    }

    m_hOldPos.resize(3 * m_pointBuffer.size(), 0.0);
    for (int i = 0; i < m_pointBuffer.size(); ++i)
    {
        m_hOldPos[3 * i + 0] = m_pointBuffer.get(i)->pos.x;
        m_hOldPos[3 * i + 1] = m_pointBuffer.get(i)->pos.y;
        m_hOldPos[3 * i + 2] = m_pointBuffer.get(i)->pos.z;
    }

    for (int i = 0; i < m_hPosW.size() / 3; ++i)
    {
        Point *p = m_pointBuffer.get(i);
        p->pos = glm::vec3(m_hPosW[3 * i], m_hPosW[3 * i + 1], m_hPosW[3 * i + 2]);
    }
    m_gridContainer.insertParticles(&m_pointBuffer);
    // 协方差矩阵 计算
    for (unsigned int i = 0; i < m_pointBuffer.size(); ++i)
    {
        glm::vec3 xi;
        xi[0] = m_hPosW[3 * i + 0];
        xi[1] = m_hPosW[3 * i + 1];
        xi[2] = m_hPosW[3 * i + 2];

        glm::vec3 xiw(0.0);
        float sumw = 0.0f;
        int cell[27 * 27]; // 2*smoothRadius的领域网格
        m_gridContainer.findTwoCells(xi, h / m_unitScale, cell);

        for (int j = 0; j < 27 * 27; j++)
        {
            if (cell[j] == -1)
                continue;
            int pndx = m_gridContainer.getGridData(cell[j]);

            while (pndx != -1)
            {
                glm::vec3 xj;
                xj[0] = m_hPosW[3 * pndx + 0];
                xj[1] = m_hPosW[3 * pndx + 1];
                xj[2] = m_hPosW[3 * pndx + 2];

                float r = glm::length(xi - xj) * m_unitScale;

                if (r < h)
                {
                    float q = 1 - r / h;
                    float wij = q * q * q;
                    xiw += xj * wij;
                    sumw += wij;
                }
                pndx = m_pointBuffer.get(pndx)->next;
            }
        }

        xiw /= sumw;

        glm::mat3 c(0.0);

        int n = 0;
        sumw = 0.0f;

        for (int j = 0; j < 27 * 27; j++)
        {
            if (cell[j] == -1)
                continue;
            int pndx = m_gridContainer.getGridData(cell[j]);

            while (pndx != -1)
            {
                glm::vec3 xj;
                xj[0] = m_hPosW[3 * pndx + 0];
                xj[1] = m_hPosW[3 * pndx + 1];
                xj[2] = m_hPosW[3 * pndx + 2];

                float r = glm::length(xi - xj) * m_unitScale;

                if (r < h)
                {
                    float q = 1 - r / h;
                    float wij = q * q * q;
                    glm::vec3 dxj = xj - xiw;

                    for (int k = 0; k < 3; ++k)
                    {
                        c[0][k] += wij * dxj[k] * dxj[0];
                        c[1][k] += wij * dxj[k] * dxj[1];
                        c[2][k] += wij * dxj[k] * dxj[2];
                    }
                    n++;
                    sumw += wij;
                }
                pndx = m_pointBuffer.get(pndx)->next;
            }
        }

        c /= sumw;

        // 奇异值分解
        float w[3], u[9], v[9];
        for (int k = 0; k < 3; ++k)
        {
            u[k * 3 + 0] = c[0][k];
            u[k * 3 + 1] = c[1][k];
            u[k * 3 + 2] = c[2][k];
        }

        RxSVDecomp3(w, u, v, 1.0e-10);

        // 特征值Σ
        glm::vec3 sigma;
        for (int j = 0; j < 3; ++j)
        {
            sigma[j] = w[j];
        }

        // 特征向量（旋转矩阵R）
        glm::mat3 R;
        for (int j = 0; j < 3; ++j)
        {
            for (int k = 0; k < 3; ++k)
            {
                R[j][k] = u[k * 3 + j];
            }
        }
        // 保存的值用于调试
        // for (int j = 0; j < 3; ++j)
        // {
        //     m_hEigen[3 * i + j] = sigma[j];
        // }

        for (int j = 0; j < 3; ++j)
        {
            for (int k = 0; k < 3; ++k)
            {
                m_hRMatrix[9 * i + 3 * j + k] = R[k][j];
            }
        }

        int ne = 25; // m_params.KernelParticles*0.8;
        float ks = 1400;
        float kn = 0.5;
        float kr = 4.0;
        if (n > ne)
        {
            for (int j = 1; j < 3; ++j)
            {
                sigma[j] = RX_MAX(sigma[j], sigma[0] / kr);
            }
            sigma *= ks;
        }
        else
        {
            for (int j = 0; j < 3; ++j)
            {
                sigma[j] = kn * 1.0;
            }
        }
        // 核变形矩阵G
        glm::mat3 G(0.0);
        glm::mat3 Sigma(1.0 / sigma.x, 0, 0, 0, 1.0 / sigma.y, 0, 0, 0, 1.0 / sigma.z);
        G = R * Sigma * glm::transpose(R);

        double max_diag = -1.0e10;
        for (int j = 0; j < 3; ++j)
        {
            for (int k = 0; k < 3; ++k)
            {
                if (G[k][j] > max_diag)
                    max_diag = G[k][j];
            }
        }

        G /= max_diag;

        // G=glm::inverse(G);
        for (int j = 0; j < 3; ++j)
        {
            for (int k = 0; k < 3; ++k)
            {
                m_hG[9 * i + 3 * j + k] = G[k][j];
            }
        }
    }
}
