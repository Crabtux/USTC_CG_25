# 6. Rasterization

> 作业步骤：
> - 查看[Kickstart文档](./kickstart.pdf)，了解光栅化作业新增了哪些东西
> - 查看[作业文档](./docs/README.md)
> - 在[项目目录](../../Framework3D/)中编写作业代码
> - 按照[作业规范](../README.md)提交作业

## 作业递交

- 递交内容：程序代码、实验报告及 `stage.usdc` 文件，见[提交文件格式](#提交文件格式)
- 递交时间：2025年4月13日（周日）晚

## 要求
- 实现 Blinn-Phong 着色模型 ([参考资料](https://learnopengl-cn.github.io/02%20Lighting/03%20Materials/))
- 实现 Shadow Mapping 算法 ([参考资料](https://learnopengl-cn.github.io/05%20Advanced%20Lighting/03%20Shadows/01%20Shadow%20Mapping/))
- 可选：实现 Percentage Close Soft Shadow ([参考资料](https://zhuanlan.zhihu.com/p/478472753))
- 可选：实现 Screen Space Ambient Occlusion ([参考资料](https://learnopengl-cn.github.io/05%20Advanced%20Lighting/09%20SSAO/#ssao))


## 目的
- 熟悉OpenGL图形API以及GLSL Shader语言
- 了解多Pass绘制方法
- 了解实时渲染中常用的屏幕空间技巧


## 提供的材料

- 基础的光栅化Pass
- 测试场景 依照上述要求和方法，根据说明文档`(1) documents`和作业框架`(2) Framework3D`的内容进行练习。

### (1)说明文档 [->](./docs/README.md)
本次作业的要求说明和一些辅助资料
### (2)作业框架 [->](../../Framework3D/)
作业的基础代码框架和测试数据。数据位于[数据文件夹](../../Framework3D/submissions/assignments/data/data_hw6)


## 提交文件格式

完成作业之后，打包四样内容即可：
  - 你的 `xxx_homework/`文件夹（拷贝并改名自 [assignments/](../../Framework3D/submissions/assignments/)，不要包含中文，详见 [F3D_kickstart.pdf](../../Framework3D/F3D%20kickstart.pdf)）
  - 节点连接信息（stage.usdc，来自框架目录下的 `Assets/` 文件夹，请一并拷贝到上边的 `xxx_homework/`文件夹里）；
  - 渲染节点连接信息（render_nodes_save.json，来自框架目录下的 `Assets/` 文件夹，请一并拷贝到上边的 `xxx_homework/`文件夹里）
  - 报告（命名为 `id_name_report6.pdf`）
  
  具体请务必**严格按照**如下的格式提交：

  ```
  ID_姓名_homework*/                // 你提交的压缩包
  ├── xxx_homework/                  
  │  ├── stage.usdc                    // （额外添加）本次作业的节点连接信息
  │  ├── render_nodes_save.json        // （额外添加）本次作业的渲染节点连接信息
  │  ├── data/                         // 你自己额外添加的测试模型和纹理
  │  │   ├── xxx.usda
  │  │   ├── yyy.usda
  │  │   ├── zzz.png
  │  │   └── ...  
  │  ├── utils/                        // 辅助代码文件
  │  │   ├── some_algorithm.h
  │  │   ├── some_algorithm.cpp
  │  │   └── ...  
  │  ├── nodes_render/                 // 本次作业你实现or修改的节点文件
  │  │   ├── node_xxxx.cpp
  │  │   └── ...  
  │  └── nodes/                        // 本次作业你实现or修改的节点文件
  │      ├── node_your_implementation.cpp
  │      ├── node_your_other_implementation.cpp
  │      └── ...  
  ├── id_name_report6.pdf                    // 实验报告
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