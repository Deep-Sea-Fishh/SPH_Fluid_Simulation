#ifndef MARCHING_CUBE_H
#define MARCHING_CUBE_H
#include <vector>
#include <string>
#include <map>
#include <glm/glm.hpp>
using namespace std;

class rxFace
{
public:
    vector<int> vert_idx;        // 顶点索引
    string material_name;        // 材质
    vector<glm::vec2> texcoords; // 像素坐标
    int attribute;               //  属性

public:
    rxFace() : attribute(0) {}

    inline int &operator[](int i) { return vert_idx[i]; }
    inline int operator[](int i) const { return vert_idx[i]; }

    // 按函数进行顶点访问
    inline int &at(int i) { return vert_idx.at(i); }
    inline int at(int i) const { return vert_idx.at(i); }

    // 更改多边形顶点的数量
    void resize(int size)
    {
        vert_idx.resize(size);
    }

    // 返回多边形顶点的数量
    int size(void) const
    {
        return (int)vert_idx.size();
    }

    // 初始化
    void clear(void)
    {
        vert_idx.clear();
        material_name = "";
        texcoords.clear();
    }
};

struct RxVertexID
{                       // 点的信息
    unsigned int newID; // 索引
    double x, y, z;     // 位置
};

typedef std::map<unsigned int, RxVertexID> ID2VertexID;

struct RxTriangle
{                            // 三角面的信息
    unsigned int pointID[3]; // 存有每个点的索引
};

typedef std::vector<RxTriangle> RxTriangleVector;

struct RxScalarField
{                         // 标量场
    unsigned int iNum[3]; // x,y,z的大小
    glm::vec3 fWidth;     // 每个网格宽度
    glm::vec3 fMin;       // 最小点坐标
};

class rxMCMesh
{
public:
    rxMCMesh();
    ~rxMCMesh();

    // 从样本量生成三角形网格
    bool CreateMeshV(float *field, glm::vec3 min_p, double h, int n[3], float threshold,
                     vector<glm::vec3> &vrts, vector<glm::vec3> &nrms, vector<rxFace> &face);

    // 从样本量生成等值面网格
    void GenerateSurfaceV(const RxScalarField sf, float *field, float threshold,
                          vector<glm::vec3> &vrts, vector<glm::vec3> &nrms, vector<int> &tris);

    // 等值面创建成功则返回true
    bool IsSurfaceValid() const { return m_bValidSurface; }

    // 删除表面
    void DeleteSurface();

    // 用于网格划分的网格大小（在未创建网格的情况下，返回值为-1）
    int GetVolumeLengths(double &fVolLengthX, double &fVolLengthY, double &fVolLengthZ);

    // 有关创建的网格的信息
    unsigned int GetNumVertices(void) const { return m_nVertices; }
    unsigned int GetNumTriangles(void) const { return m_nTriangles; }
    unsigned int GetNumNormals(void) const { return m_nNormals; }

private:
    unsigned int m_nVertices;  // 点的个数
    unsigned int m_nNormals;   // 法线个数
    unsigned int m_nTriangles; // 三角面个数

    ID2VertexID m_i2pt3idVertices;      // 形成等值面的顶点列表(以边索引为key，等值面的点为value)
    RxTriangleVector m_trivecTriangles; // 形成三角形的顶点列表
    RxScalarField m_Grid;               // 标量场信息

    const float *m_ptScalarField; // 保存标量值的样本量

    float m_tIsoLevel;    // 阈值
    bool m_bValidSurface; // 表面是否生成成功

    // 边id
    unsigned int GetEdgeID(unsigned int nX, unsigned int nY, unsigned int nZ, unsigned int nEdgeNo);

    // 顶点ID
    unsigned int GetVertexID(unsigned int nX, unsigned int nY, unsigned int nZ);

    // 计算边缘上的等值面
    RxVertexID CalculateIntersection(unsigned int nX, unsigned int nY, unsigned int nZ,
                                     unsigned int nEdgeNo);

    // 通过网格边缘两端的隐式函数值的线性插值计算等值点
    RxVertexID Interpolate(double fX1, double fY1, double fZ1, double fX2, double fY2, double fZ2,
                           float tVal1, float tVal2);

    // 以输出形式存储顶点和网格几何信息
    void RenameVerticesAndTriangles(vector<glm::vec3> &vrts, unsigned int &nvrts, vector<int> &tris, unsigned int &ntris);

    // 顶点法线计算
    void CalculateNormals(const vector<glm::vec3> &vrts, unsigned int nvrts, const vector<int> &tris, unsigned int ntris, vector<glm::vec3> &nrms, unsigned int &nnrms);
};

#endif