# 10. Character Animation

> 作业步骤：
> - 查看[文档](./documents/README.md)
> - 在[作业框架](../../Framework3D)中编写作业代码
> - 按照[作业规范](../README.md)提交作业

## 作业递交

- 递交内容：程序代码及实验报告，见[提交文件格式](#提交文件格式)
- 递交时间：2025年5月18日（周日）晚

## 要求

- 实现骨骼的关节变换
- 实现蒙皮顶点更新
- (Optional) 基于布料仿真实现衣服运动
  
## 目的

- 了解骨骼动画的原理
- 理解矩阵变换


## 提供的材料

- 包含骨骼和mesh数据读取的代码框架
- 骨骼动画文件和衣服布料文件


依照上述要求和方法，根据说明文档`(1) documents`和作业框架`(2) Framework3D`的内容进行练习。

### (1) 说明文档 `documents` [->]() 

### (2) 作业项目 `Framework3D` [->](../../Framework3D/) 

## 提交文件格式

文件命名为 `ID_姓名_Homework*.rar/zip`，其中包含：

  - 你的 `xxx_homework/`文件夹（拷贝并改名自 [assignments/](../../Framework3D/submissions/assignments/)，不要包含中文，详见 [F3D_kickstart.pdf](../../Framework3D/F3D%20kickstart.pdf)）
  - 报告（命名为 `id_name_report10.pdf`）
  
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
  │  │   ├── character_animation/                // **重点要修改的文件夹！**
  │  │   ├── some_algorithm.h
  │  │   ├── some_algorithm.cpp
  │  │   └── ...  
  │  └── nodes/                        // 本次作业你实现or修改的节点文件
  │      ├── node_your_implementation.cpp
  │      ├── node_your_other_implementation.cpp
  │      └── ...  
  ├── id_name_report10.pdf                    // 实验报告
  ├── CMakeLists.txt                // CMakeLists.txt 文件不要删除
  └── ...                           // 其他补充文件（可以提供直观的视频或者 gif!）
  ```
