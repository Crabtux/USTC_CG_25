# 7. Path Tracing

> 作业步骤：
> - 查看[文档](./rtfd.pdf)
> - 在`Framework3D/Framework3D/source/Plugins/hd_USTC_CG_Embree/`中编写作业代码
> - 按照[作业规范](../README.md)提交作业
## **ATTENTION**!!!!
本次作业的编写需要大家直接在框架里面进行修改。具体而言是修改`Framework3D\Framework3D\source\Plugins\hd_USTC_CG_Embree`下的相应内容。这个文件夹里面所有东西你都是可以进行修改的，提交的时候需要提交整个文件夹！

## 作业递交

- 递交内容：程序代码（hd_USTC_CG_Embree下全部文件）及实验报告，见[提交文件格式](#提交文件格式)
- 递交时间：2025年4月20日（周日）晚

## 要求

- 完成矩形光源相关内容（相交计算，采样计算，Irradiance计算）
- 使用直接光照积分器验证结果
- 路径追踪算法
- Russian Roulette
- (Optional) 添加一种材质，对材质进行重要性采样，并进行多重重要性采样，与单种采样的结果进行比较
- (Optional) 透明支持（需要考虑IOR）

## 目的

- 熟悉渲染过程中多个概率空间的转换
- 熟悉路径追踪算法
- 了解多重重要性采样


## 提供的材料

- 几类光源的重要性采样参考
- 场景相交的数据结构和类型
- 理想漫反射采样支持
- 测试场景
- 
依照上述要求和方法，根据说明文档`(1) documents`和作业框架`(2) Framework3D`的内容进行练习。

### (1) 说明文档 `documents` [->](./rtfd.pdf) 

### (2) 作业项目 `Framework3D` [->](../../Framework3D/) 

作业的基础代码框架和测试数据。

测试数据链接(去年的圣遗物，今年还能接着用)：链接：https://rec.ustc.edu.cn/share/18163800-fe1c-11ee-b6af-f9c116738547

## 提交文件格式

文件命名为 `ID_姓名_Homework*.rar/zip`，其中包含：

```
ID_姓名_Homework*/
├── hd_USTC_CG_Embree/            // 渲染器全部代码
│   ├── xxx.h
│   ├── xxx.cpp
|   └── ...
├── report.pdf                    // 实验报告
└── ...                           // 其他补充文件
```
本次作业提交可以不用带stage.usdc文件！
