#ifndef FLUID_SYSTEM_H
#define FLUID_SYSTEM_H
#define SUF
#include <glm/glm.hpp>
#include "box.h"
#include "neighborList.h"
#include "marchingCube.h"
#include <mutex>
class FluidSystem
{
public:
    FluidSystem();
    void init(unsigned int maxPointCounts,
              const glm::vec3 &wallBox_min, const glm::vec3 &wallBox_max,
              const glm::vec3 &initFluidBox_min, const glm::vec3 &initFluidBox_max,
              const glm::vec3 &gravity)
    {
        _init(maxPointCounts, fBox3(wallBox_min, wallBox_max),
              fBox3(initFluidBox_min, initFluidBox_max), gravity);
    }
    // 获取点的尺寸（字节）
    unsigned int getPointStride() const { return sizeof(Point); }
    // 获取点的数量
    unsigned int getPointCounts() const { return m_pointBuffer.size(); }
    // 获取流体点缓存
    const Point *getPointBuf() const { return m_pointBuffer.get(0); }
    // 逻辑桢
    void tick();

    std::vector<float> &getPosData() { return posData; }

    ~FluidSystem();

    // 复制缓存
    void CuCopyArrayToDevice(float *device, const float *host, int offset, int size);

    // 隐式函数值计算
    virtual double GetImplicit(double x, double y, double z);
    virtual void CalImplicitField(int n[3], glm::vec3 minp, glm::vec3 d, float *hF);
    virtual void CalImplicitFieldDevice(int n[3], glm::vec3 minp, glm::vec3 d, float *dF);

    // 使用metaball隐式函数值
    double CalColorField(double x, double y, double z);

    float *getPointPosBuf() { return &posData[0]; } // 获取点位置缓存
    float *getPolygonBuf();                         // 获取三角面
    float *getSufPosBuf() { return &m_vrts[0].x; }  // 获取表面点
    float *getSufPosNorBuf();                       // 获取表面点以及法线
    unsigned int *getSufTriIdxBuf();                // 获取三角面索引

    void clearSuf()
    {
        m_nrms.clear();
        m_nrms.clear();
        m_face.clear();
    }
    int getSufVrtBufNum() { return (int)m_vrts.size(); }
    int getPolyNum();

private:
    // 初始化系统
    void
    _init(unsigned int maxPointCounts, const fBox3 &wallBox, const fBox3 &initFluidBox, const glm::vec3 &gravity);

    void _computeGradWValues();
    void _computeDensityErrorFactor();
    // 计算密度，压力以及相邻关系
    void _computerPressure();
    // 计算加速度
    void _computerForce(int l, int r);
    // 创建初始液体块
    void _addFluidVolume(const fBox3 &fluidBox, float spacing);

    glm::vec3 _boundaryForce(Point *p);
    void _collisionHandling(glm::vec3 &pos, glm::vec3 &vel);
    void _computePredictedDensityAndPressure(int L, int R);
    void _updatePosAndVel(int L, int R);
    void _computeCorrectivePressureForce(int L, int R);
    void _computePosVel(int L, int R);
    void updatePosAndVel();
    void buildNeighborList();
    float _computeNeighbor(int i);
    void computeForce();
    void predictionCorrectionStep();
    // 数据成员
    PointBuffer m_pointBuffer;
    PointBuffer m_wallBuffer;
    GridContainer m_gridContainer;
    NeighborTable m_neighborTable;

    // 点位置缓存数据(x,y,z)
    std::vector<float> posData;

    // SPH光滑核
    float m_kernelPoly6;
    float m_kernelSpiky;
    float m_kernelViscosity;

    // 其他参数
    float m_pointDistance;     // 半径
    float m_unitScale;         // 尺寸单位
    float m_viscosity;         // 粘性
    float m_restDensity;       // 静态密度
    float m_pointMass;         // 质量
    float m_wallMass;          // 质量
    float m_smoothRadius;      // 光滑核半径
    float m_gasConstantK;      // 气体常量k
    float m_boundaryStiffness; // 边界刚性
    float m_boundaryDampening; // 边界阻尼
    float m_speedLimiting;     // 速度限制
    glm::vec3 m_gravityDir;    // 重力矢量
    float minLoops = 3;
    float maxLoops = 30;
    float timeStep = 0.001f;
    float m_densityErrorFactor;
    bool density_error_too_large;
    float max_predicted_density;
    int m_rexSize[3]; // 网格尺寸

    fBox3 m_sphWallBox;

    rxMCMesh m_mcMesh; // mc类型的成员

    float *m_field;               // 密度场
    std::vector<float> m_polyBuf; // 三角面缓存

    // 表面信息
    vector<glm::vec3> m_vrts; // 表面点坐标
    vector<glm::vec3> m_nrms; // 点法线
    vector<rxFace> m_face;    // 表面三角面
    vector<glm::vec3> m_vrt_nrms;
    vector<unsigned int> m_tri_idx;

    float m_thre; // 隐函数阈值

    int threadCnt;
    std::mutex mtx;
};
#endif