#include "box.h"
#include <algorithm>
#include <iostream>
int GridContainer::getGridData(int gridIndex)
{
    if (gridIndex < 0 || gridIndex >= m_gridData.size())
        return -1;
    return m_gridData[gridIndex];
}

int GridContainer::getGridCellIndex(const float px, const float py, const float pz)
{
    int gx = (int)((px - m_GridMin.x) * m_GridDelta.x);
    int gy = (int)((py - m_GridMin.y) * m_GridDelta.y);
    int gz = (int)((pz - m_GridMin.z) * m_GridDelta.z);
    return (gz * m_GridRes.y + gy) * m_GridRes.x + gx;
}

void GridContainer::init(const fBox3 &box, float sim_scale, float cell_size, float border, int *rex)
{
    float world_cellsize = cell_size / sim_scale;

    m_GridMin = box.min;
    m_GridMin -= border;
    m_GridMax = box.max;
    m_GridMax += border;
    m_GridSize = m_GridMax;
    m_GridSize -= m_GridMin;

    m_GridCellSize = world_cellsize;
    // 网格规格
    m_GridRes.x = (int)ceil(m_GridSize.x / world_cellsize);
    m_GridRes.y = (int)ceil(m_GridSize.y / world_cellsize);
    m_GridRes.z = (int)ceil(m_GridSize.z / world_cellsize);

    // 将网格大小调整为单元格大小的倍数
    m_GridSize.x = m_GridRes.x * cell_size / sim_scale;
    m_GridSize.y = m_GridRes.y * cell_size / sim_scale;
    m_GridSize.z = m_GridRes.z * cell_size / sim_scale;

    // 计算偏移量
    m_GridDelta = m_GridRes;
    m_GridDelta /= m_GridSize;

    int gridTotal = (int)(m_GridRes.x * m_GridRes.y * m_GridRes.z);

    rex[0] = m_GridRes.x * 8;
    rex[1] = m_GridRes.y * 8;
    rex[2] = m_GridRes.z * 8;

    m_gridData.resize(gridTotal);
    std::cout << cell_size << " " << world_cellsize << std::endl;
    std::cout << m_GridMax.x << " " << m_GridMax.y << " " << m_GridMax.z << std::endl;
}

void GridContainer::insertParticles(PointBuffer *pointBuffer)
{
    std::fill(m_gridData.begin(), m_gridData.end(), -1);
    Point *p = pointBuffer->get(0);
    for (unsigned int n = 0; n < pointBuffer->size(); n++, p++)
    {
        int gs = getGridCellIndex(p->pos.x, p->pos.y, p->pos.z);

        // 每个网格内的点划分为一个链表(m_gridData[gs]是该网格中链表的头节点)
        if (gs >= 0 && gs < m_gridData.size())
        {
            p->next = m_gridData[gs];
            m_gridData[gs] = n;
        }
        else
            p->next = -1;
    }
}

int GridContainer::findCell(const glm::vec3 &p)
{
    int gc = getGridCellIndex(p.x, p.y, p.z);
    if (gc < 0 || gc >= m_gridData.size())
        return -1;
    return gc;
}

void GridContainer::findCells(const glm::vec3 &p, float radius, int *gridCell)
{
    for (int i = 0; i < 27; i++)
        gridCell[i] = -1;

    int idx = getGridCellIndex(p.x, p.y, p.z);
    int iz = idx % m_GridRes.z;
    int iy = (idx / m_GridRes.z) % m_GridRes.y;
    int ix = idx / (m_GridRes.y * m_GridRes.z);
    int cnt = 0;
    for (int i = -1; i < 2; i++)
    {
        for (int j = -1; j < 2; j++)
        {
            for (int k = -1; k < 2; k++)
            {
                int x = ix + i, y = iy + j, z = iz + k;
                if (x < 0 || y < 0 || z < 0 || x >= m_GridRes.x || y >= m_GridRes.y || z >= m_GridRes.z)
                    continue;
                int index = x * (m_GridRes.y * m_GridRes.z) + y * m_GridRes.z + z;
                gridCell[cnt++] = index;
            }
        }
    }
}

void GridContainer::findTwoCells(const glm::vec3 &p, float radius, int *gridCell)
{
    for (int i = 0; i < 27 * 27; i++)
        gridCell[i] = -1;

    int idx = getGridCellIndex(p.x, p.y, p.z);
    int iz = idx % m_GridRes.z;
    int iy = (idx / m_GridRes.z) % m_GridRes.y;
    int ix = idx / (m_GridRes.y * m_GridRes.z);
    int cnt = 0;
    for (int i = -2; i < 2; i++)
    {
        for (int j = -2; j < 3; j++)
        {
            for (int k = -2; k < 3; k++)
            {
                int x = ix + i, y = iy + j, z = iz + k;
                if (x < 0 || y < 0 || z < 0 || x >= m_GridRes.x || y >= m_GridRes.y || z >= m_GridRes.z)
                    continue;
                int index = x * (m_GridRes.y * m_GridRes.z) + y * m_GridRes.z + z;
                gridCell[cnt++] = index;
            }
        }
    }
}
