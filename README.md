# Indoor-Scene-Reconstruction

**轻量级 C++/OpenGL 室内场景几何重建与物理渲染引擎**

本项目主要探索从二维户型图到三维室内场景的自动化生成流程，实现了图像结构提取、二维坐标解析、三维 Mesh 构建、OpenGL 实时渲染以及基础光照与纹理映射。

---

## 项目效果

### 已实现功能
* **户型图边缘提取**
* **霍夫线段检测**
* **墙体结构解析**
* **2D 坐标到 3D 坐标转换**
* **三维 Mesh 自动生成**
* **OpenGL 场景渲染**
* **基础光照系统**
* **纹理映射**
* **自由视角相机移动**

## 技术栈
* **C++**
* **OpenCV**
* **OpenGL**
* **GLSL Shader**
* **GLFW**
* **GLM**
* **GLAD**

## 项目流程

### 1. 图像预处理
使用 OpenCV 对户型图进行灰度化、边缘检测与霍夫线变换，提取房间结构线段。

### 2. 几何结构解析
根据线段信息计算墙体厚度与顶点坐标，并生成二维拓扑结构。

### 3. 三维场景生成
将二维坐标映射到三维空间，构建墙体 Mesh 数据并生成室内场景。

### 4. OpenGL 渲染
使用 OpenGL 完成场景绘制，并实现：
* VAO/VBO/EBO 缓冲管理
* Shader 渲染流程
* Phong 光照模型
* 纹理映射

### 5. Shader 优化
在大面积平面高光渲染过程中，尝试使用 dFdx/dFdy 对片元法线进行重建，以减少部分插值带来的光照畸变问题。

## 项目结构
```text
Indoor-Scene-Reconstruction/
├── assets/              # 纹理与户型数据
├── docs/                # 数学推导与开发记录
├── ThirdParty/opengl/   # GLFW / GLAD / GLM
├── *.cpp / *.h          # 核心模块
├── *.vert / *.frag      # GLSL Shader
└── main.cpp
