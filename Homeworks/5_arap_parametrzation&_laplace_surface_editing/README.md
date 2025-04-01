# 5. ARAP参数化 ARAP Parameterzation & Laplace表面编辑 Laplacian Surface Editing

> 作业步骤：
> - 查看[可选作业一文档](5.1_arap_parameterization/README.md)或者[可选作业二文档](5.2_laplacian_surface_editing/README.md)
> - 在[项目目录](../../Framework3D/)中编写作业代码
> - 按照[作业规范](../README.md)提交作业

## 作业递交

- 递交内容：程序代码、实验报告及 `stage.usdc` 文件，见[提交文件格式](#提交文件格式)
- 递交时间：2025年3月30日（周日）晚

## 要求

以下两个作业任选其一完成：

### 可选作业一：
- 实现论文 [A Local/Global Approach to Mesh Parameterization](https://cs.harvard.edu/~sjg/papers/arap.pdf) 中介绍的 ARAP (As-rigid-as-possible) 网格参数化方法，主要原理见[作业课件](https://rec.ustc.edu.cn/share/1c0d1d10-db2e-11ef-b910-f95ea2c8844c)

- （Optional）实现论文中的另外两种参数化
  - ASAP（As-similar-as-possible）参数化算法
  - Hybrid 参数化算法
- 使用测试纹理和网格检验实验结果

### 可选作业二：
- 实现论文 [Laplacian Surface Editing](https://graphics.stanford.edu/courses/cs468-07-winter/Papers/sgp2004_laplace.pdf) 中介绍的Laplace表面编辑方法，主要原理见[作业课件](https://rec.ustc.edu.cn/share/1c0d1d10-db2e-11ef-b910-f95ea2c8844c)
- 实时拖动区域显示结果
- 不同权重的Laplacian的选取，至少实现均匀权重和余切权重

- （Optional）实现一个局部的Laplace表面编辑方法，即可以在局部的区域进行表面编辑。

## 目的

- 继续学习网格数据结构和编程
- 巩固大型稀疏线性方程组的求解方法

可选作业一：
- 对各种参数化算法（作业4、作业5）进行比较
- 了解非线性优化
- 理解并实现（二阶）矩阵的 SVD 分解

可选作业二：
- 了解网格编辑和交互
- 使用矩阵预分解提高计算效率
- 了解一些基础的计算最短路径，搜索点的算法


## 提供的材料

依照上述要求和方法，根据说明文档`(1) documents`和作业框架`(2) Framework3D`的内容进行练习。

### (1) 说明文档一 `documents_1` [->](5.1_arap_parameterization/) 

###    说明文档二 `documents_2` [->](5.2_laplacian_surface_editing/)

本次作业的要求说明和一些辅助资料

### (2) 作业项目 `Framework3D` [->](../../Framework3D/) 

作业的基础代码框架

## 提交文件格式

完成作业之后，打包三样内容即可：
  - 你的 `xxx_homework/`文件夹（拷贝并改名自 [assignments/](../../Framework3D/submissions/assignments/)，不要包含中文，详见 [F3D_kickstart.pdf](../../Framework3D/F3D%20kickstart.pdf)）
  - 节点连接信息（stage.usdc，来自框架目录下的 `Assets/` 文件夹，请一并拷贝到上边的 `xxx_homework/`文件夹里）；
  - 报告（命名为 `id_name_report5.pdf`）
  
  具体请务必严格按照如下的格式提交：

  ```
  ID_姓名_homework*/                // 你提交的压缩包
  ├── xxx_homework/                  
  │  ├── stage.usdc                    // （额外添加）本次作业的节点连接信息
  │  ├── data/                         // 测试模型和纹理
  │  │   ├── xxx.usda
  │  │   ├── yyy.usda
  │  │   ├── zzz.png
  │  │   └── ...  
  │  ├── utils/                        // 辅助代码文件
  │  │   ├── some_algorithm.h
  │  │   ├── some_algorithm.cpp
  │  │   └── ...  
  │  └── nodes/                        // 本次作业你实现or修改的节点文件
  │      ├── node_your_implementation.cpp
  │      ├── node_your_other_implementation.cpp
  │      └── ...  
  ├── id_name_report5.pdf                    // 实验报告
  ├── CMakeLists.txt                // CMakeLists.txt 文件不要删除
  └── ...                           // 其他补充文件
  ```

### 注意事项

- 导入数据（网格和纹理）时使用**相对路径**，例如，将你的数据放在可执行文件目录下，直接通过 `FilePath = 'xxx.usda'` 或者 `FilePath = 'zzz.png'` 访问，或者定位到作业目录的 `data/` 文件夹中；
- 在 `node_your_implementation.cpp` 等文件中使用
  ```cpp
  #include "some_algorithm.h"
  ```
  包含你的辅助代码；其中some_algorithm.h & some_algorithm.cpp直接放在utils中。
- 如果除了添加 `utils/` 和 `node_your_implementation.cpp`，你还**对框架有其他修改**，就**打包上传所有代码**。
- **尽量不要动框架的东西！！！！**