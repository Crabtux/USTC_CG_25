# 8. Mass Spring

> 作业步骤：
> - 查看[文档](./documents/README.md)
> - 在[作业框架](../../Framework3D)中编写作业代码，主要是 [MassSpring](../../Framework3D/submissions/assignments/utils/mass_spring/) 文件夹
> - 按照[作业规范](../README.md)提交作业

## 作业递交

- 递交内容：程序代码及实验报告，见[提交文件格式](#提交文件格式)
- 递交时间：2025年4月11日（周日）晚

## 要求

- 实现基础的半隐式与隐式的质点弹簧仿真
- 比较不同方法、不同参数设置下的仿真效果
- (Optional) 实现基于惩罚力的布料与球之间的碰撞处理
- (Optional) 复现[Liu et al. Siggraph 2013] 提出的基于Local-Global思想的质点弹簧系统加速算法
- (Optional) 体网格的质点弹簧仿真

## 目的

- 熟悉半隐式与隐式时间积分在仿真中的应用
- 了解弹簧质点系统这一经典的基于物理的仿真方法


## 提供的材料

- 代码框架和节点图
- 测试用的几何和贴图文件


依照上述要求和方法，根据说明文档`(1) documents`和作业框架`(2) Framework3D`的内容进行练习。

### (1) 说明文档 `documents` [->]() 

### (2) 作业项目 `Framework3D` [->](../../Framework3D/) 

## 提交文件格式

文件命名为 `ID_姓名_Homework*.rar/zip`，其中包含：

  - 你的 `xxx_homework/`文件夹（拷贝并改名自 [assignments/](../../Framework3D/submissions/assignments/)，不要包含中文，详见 [F3D_kickstart.pdf](../../Framework3D/F3D%20kickstart.pdf)
  - 报告（命名为 `id_name_report8.pdf`）
  
  具体请务必严格按照如下的格式提交：

  ```
  ID_姓名_homework*/                // 你提交的压缩包
  ├── xxx_homework/                  
  │  ├── data/                         // 测试模型和纹理
  │  │   ├── xxx.usda
  │  │   ├── yyy.usda
  │  │   ├── zzz.png
  │  │   └── ...  
  │  ├── utils/                        // 辅助代码文件
  │  │   ├── mass_spring/              // **重点要修改的文件夹！**
  │  │   ├── some_algorithm.h
  │  │   ├── some_algorithm.cpp
  │  │   └── ...  
  │  └── nodes/                        // 本次作业你实现or修改的节点文件
  │      ├── node_your_implementation.cpp
  │      ├── node_your_other_implementation.cpp
  │      └── ...  
  ├── id_name_report8.pdf                    // 实验报告
  ├── CMakeLists.txt                // CMakeLists.txt 文件不要删除
  └── ...                           // 其他补充文件（可以提供直观的视频或者 gif!）
  ```
