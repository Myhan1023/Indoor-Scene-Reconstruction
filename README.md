# Indoor-Scene-Reconstruction

**轻量级 C++/OpenGL 室内场景几何重建与物理渲染引擎**

基于 C++、OpenCV 与 OpenGL 开发轻量级室内场景重建系统，实现从二维户型线稿到三维场景的自动化生成与实时渲染。系统包含图像预处理、几何结构解析、Mesh 构建、光照渲染与纹理映射等核心模块。

---

## 核心技术特性 (Key Features)

### 1. 2D 语义解析与几何升维 (2D-to-3D Reconstruction)
* **特征提取**：利用 OpenCV 库对原始户型线稿进行预处理，通过**霍夫线变换（Hough Line Transform）**实现像素级边缘到几何线段的语义提取。
* **自动化建模**：独立开发**法向量膨胀算法**。系统根据提取的 2D 拓扑结构，自动化计算墙体厚度、顶点坐标及法线方向，动态构建三维多边形网格（Mesh Generation）。

### 2. 基于物理的实时渲染管线 (High-Performance Rendering)
* **底层内存管理**：手动管理 VAO、VBO、EBO 及纹理缓冲，优化 CPU-GPU 之间的数据总线通信。
* **物理光照模型**：完整实现 **Phong 反射模型**，支持环境光、漫反射及镜面高光的实时演算。
* **纹理采样优化**：支持多重纹理映射（Texture Mapping），实现室内地板与墙面的高精度 UV 坐标对齐。

### 3. GPU 片段着色器优化 (Shader Optimization)
* **偏导数抗畸变方案**：针对大面积低采样网格在高光下产生的插值畸变（Pinch Artifacts），本项目在 Fragment Shader 中通过 **GPU 屏幕空间偏导数 (`dFdx`, `dFdy`)** 实时重构面法线。

---

## 算法演进记录 (Project Milestones)

| 阶段 | 技术节点 | 渲染效果预览 |
| :--- | :--- | :--- |
| **Phase 1** | 2D 线段语义提取 (OpenCV) | ![线段提取](docs/屏幕截图_2026-04-15_144754.png) |
| **Phase 2** | 3D 拓扑网格自动化生成 | ![网格生成](docs/屏幕截图_2026-04-25_115607.png) |
| **Phase 3** | **核心攻关**：GPU 偏导数光照优化 | ![光照对比](docs/屏幕截图_2026-05-08_115222.png)![未用偏导数](docs/屏幕截图_2026-05-08_112936.png) |
| **Phase 4** | 纹理映射与系统底座竣工 | ![最终效果](docs/屏幕截图_2026-05-10_193704.png) |

---

## 数学推导 (Mathematical Derivations)

* **坐标系变换**: 实现了从像素空间坐标到世界空间 NDC 坐标的矩阵变换推导。
* **法线权重计算**: 解决了多面共用顶点在光照计算中的权重衰减问题。
* **相机模型**: 基于欧拉角与三角函数手搓了自由视角漫游系统（Pitch/Yaw）。

> **详细的 13 页数学推导原稿及算法逻辑图，请查阅 `docs/` 目录。**

---

## 目录结构 (Directory Structure)

```text
Indoor-Scene-Reconstruction/
├── assets/              # 纹理资产 (Texture) 与 JSON 户型拓扑数据
├── docs/                # 13页核心算法数学推导手稿与研发里程碑截图
├── ThirdParty/opengl/   # 源码级集成的第三方图形库 (GLFW, GLM, GLAD)
├── *.cpp / *.h          # 引擎核心逻辑 (Camera, Mesh, Parser, Transformer, etc.)
├── *.vert / *.frag      # GLSL 物理光照与顶点着色器脚本
└── main.cpp             # 渲染引擎入口程序
