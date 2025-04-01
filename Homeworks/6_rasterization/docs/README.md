# Shader编程

欢迎来到GPU编程的世界！

GPGPU被广泛使用在各种任务中，是目前火热的人工智能领域最重要的硬件资源。在本次作业中，我么会利用GPU最开始设计出来的目的——图形计算。

遇到问题可以查看[Q&A](./QA.md)。

## 本次作业

我们需要完成：

### Blinn Phong着色模型
1. 首先连接光栅化节点的输出，观察不同连接的结果。
2. 修改延迟着色节点和其shader以实现着色模型
3. 在光栅化节点中实现法线贴图。[法线贴图资料](https://learnopengl-cn.github.io/05%20Advanced%20Lighting/04%20Normal%20Mapping/)。

### Shadow Mapping
1. 修改shadow mapping节点以及对应的着色器（如果你想要对除了SphereLight以外的其他光源进行支持）
2. 修改延迟着色节点以及对应的着色器，实现对Shadow map的使用

### PCSS (Optional)
修改或拷贝一份blinn_phong.fs，按注释和参考资料完成PCSS
### SSAO (Optional)
修改SSAO节点和对应的着色器，读取深度和光照的结果，实现SSAO


## OpenGL

**图形API：** 我们需要通过图形API来与GPU交换信息。GPU计算的输入输出都会通过图形API来在内存和显存之间传递。

OpenGL是目前广泛使用的一种图形API。由于它诞生较早，近些年发展不是非常激进，因此它具备的一个优势是支持最为广泛，可以认为几乎所有的GPU都会对OpenGL有比较完善的支持。它的另一个优势是逻辑较为简单，适合初学者上手。


## Shader

Shader是一段在GPU上执行的代码。

Shader有多种类型。最早发展的是顶点着色器（Vertex Shader）和面元着色器（Fragment Shader）。它们构成的管线能够读取顶点的描述信息，将读取的结果进行光栅化，并且进行着色。


## 现代图形API

除了OpenGL，我们通常所说的现代图形API还包括DirectX 12以及Vulkan。此类API相比OpenGL主要的区别在于执行流程的改变。OpenGL是一个全局的状态机模型。在使用时我们的每一个指令都需要进行“绑定”的操作，每一个操作都需要在CPU和GPU之间进行同步，即CPU侧发出指令，GPU侧进行计算，完成后将结果反馈给CPU，CPU再执行下一个指令。此过程相当低效，并且严重阻碍了在渲染使用多线程调用图形API（设想我们有数十万个动态小模型需要渲染）。

Vulkan和DirectX都有非常清晰的异构执行模型。

Apple推出了Matal图形API。感兴趣的同学可以查找关于此API的相关资料。
