# Day01 - OpenGL 入门：第一个三角形

> 本文档配合 `main.cpp` 阅读，系统讲解现代 OpenGL 的核心概念。

---

## 目录

1. [基础库介绍](#1-基础库介绍)
2. [核心概念](#2-核心概念)
3. [着色器系统](#3-着色器系统)
4. [缓冲对象](#4-缓冲对象)
5. [渲染循环](#5-渲染循环)
6. [代码流程图](#6-代码流程图)
7. [关键函数速查表](#7-关键函数速查表)

---

## 1. 基础库介绍

### 1.1 GLFW - 窗口与输入管理

**GLFW** 是一个轻量级的跨平台库，专门用于：

- 创建窗口和 OpenGL 上下文
- 处理键盘、鼠标输入
- 管理多显示器

```cpp
glfwInit();                    // 初始化 GLFW
glfwCreateWindow(800, 600, "LearnOpenGL", nullptr, nullptr);  // 创建窗口
glfwMakeContextCurrent(window); // 设置当前上下文
glfwTerminate();               // 清理资源
```

**为什么需要 GLFW？**

OpenGL 本身只负责图形渲染，不管窗口创建和输入处理。GLFW 填补了这个空白，让你能专注于图形编程。

---

### 1.2 GLAD - OpenGL 函数加载器

**GLAD** 是一个 OpenGL 加载库，它的作用是：

- 在运行时加载 OpenGL 函数指针
- 处理不同平台的差异
- 让你能使用现代 OpenGL 函数

```cpp
// 必须在创建 OpenGL 上下文之后调用
gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
```

**为什么需要 GLAD？**

OpenGL 函数不像标准库函数那样直接可用。显卡驱动在运行时才提供这些函数的地址，GLAD 帮你自动获取这些地址。

**类比理解：**
- GLFW = 建房子（创建窗口）
- GLAD = 接通水电（加载 OpenGL 函数）
- OpenGL = 在房子里画画（实际渲染）

---

## 2. 核心概念

### 2.1 OpenGL 上下文 (Context)

**上下文**是 OpenGL 的"工作环境"，包含了：

- 所有 OpenGL 状态
- 创建的所有对象（纹理、缓冲区等）
- 当前绑定的各种资源

```cpp
glfwMakeContextCurrent(window);  // 激活这个窗口的上下文
```

**重要规则：**
- 每个线程同一时间只能有一个活跃上下文
- OpenGL 调用总是作用于当前上下文
- 没有上下文，OpenGL 函数调用无效

---

### 2.2 Core Profile vs Compatibility Profile

| 特性 | Core Profile | Compatibility Profile |
|------|-------------|----------------------|
| 固定管线 (glBegin/glEnd) | ❌ 不支持 | ✅ 支持 |
| 着色器 | ✅ 必须使用 | ✅ 可选 |
| 性能 | 更好 | 一般 |
| macOS 支持 | ✅ 唯一选择 | ❌ 不支持 |
| 学习曲线 | 陡峭 | 平缓 |

```cpp
glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 使用核心模式
glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // macOS 必需
```

**为什么选择 Core Profile？**

1. macOS 只支持 Core Profile
2. 强制你学习现代 OpenGL
3. 性能更好，更接近 GPU 真实工作方式

---

### 2.3 渲染管线 (Rendering Pipeline)

渲染管线是数据从顶点到像素的处理流程：

```
┌─────────────┐
│  顶点数据    │  ← 你提供的三角形坐标
└──────┬──────┘
       ▼
┌─────────────┐
│ 顶点着色器   │  ← 处理每个顶点（位置变换）
└──────┬──────┘
       ▼
┌─────────────┐
│  图元装配    │  ← 把顶点组装成三角形
└──────┬──────┘
       ▼
┌─────────────┐
│  光栅化     │  ← 把三角形转换成像素
└──────┬──────┘
       ▼
┌─────────────┐
│ 片段着色器   │  ← 决定每个像素的颜色
└──────┬──────┘
       ▼
┌─────────────┐
│  帧缓冲     │  ← 最终显示在屏幕上
└─────────────┘
```

**可编程阶段：** 顶点着色器、片段着色器（你必须自己写）

**固定阶段：** 图元装配、光栅化（GPU 自动处理）

---

## 3. 着色器系统

### 3.1 什么是着色器？

着色器是运行在 **GPU** 上的小程序，使用 **GLSL** (OpenGL Shading Language) 编写。

**特点：**
- 并行执行：GPU 同时处理成千上万个顶点/像素
- 独立运行：每个着色器实例不知道其他实例的存在
- 输入输出：通过特定变量传递数据

---

### 3.2 顶点着色器 (Vertex Shader)

**作用：** 处理每一个顶点，主要做坐标变换。

```glsl
#version 330 core                    // 使用 GLSL 3.30 核心版本
layout (location = 0) in vec3 aPos;  // 输入：顶点位置，location=0 对应 VAO 的属性 0
void main() {
    gl_Position = vec4(aPos, 1.0);   // 输出：裁剪空间坐标
}
```

**关键字解释：**

| 关键字 | 含义 |
|--------|------|
| `#version 330 core` | GLSL 版本声明，330 对应 OpenGL 3.3 |
| `layout (location = 0)` | 指定属性位置，与 `glVertexAttribPointer` 的第一个参数对应 |
| `in` | 输入变量，从 VBO 读取 |
| `vec3` | 三维向量类型 (x, y, z) |
| `vec4` | 四维向量类型 (x, y, z, w) |
| `gl_Position` | 内置输出变量，必须赋值，表示顶点的最终位置 |

---

### 3.3 片段着色器 (Fragment Shader)

**作用：** 决定每个像素（片段）的最终颜色。

```glsl
#version 330 core
out vec4 FragColor;                        // 输出：像素颜色
void main() {
    FragColor = vec4(0.95, 0.6, 0.1, 1.0);  // RGBA：橙黄色，不透明
}
```

**关键字解释：**

| 关键字 | 含义 |
|--------|------|
| `out` | 输出变量，写入帧缓冲 |
| `vec4` | RGBA 四分量颜色 |
| 颜色范围 | 0.0 ~ 1.0（不是 0 ~ 255） |

---

### 3.4 着色器程序的编译与链接

```cpp
// 1. 创建着色器对象
unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

// 2. 附加源代码
glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);

// 3. 编译着色器
glCompileShader(vertexShader);
glCompileShader(fragmentShader);

// 4. 创建程序并链接
unsigned int shaderProgram = glCreateProgram();
glAttachShader(shaderProgram, vertexShader);
glAttachShader(shaderProgram, fragmentShader);
glLinkProgram(shaderProgram);

// 5. 删除不再需要的着色器对象
glDeleteShader(vertexShader);
glDeleteShader(fragmentShader);

// 6. 使用程序
glUseProgram(shaderProgram);
```

**流程类比：**
- `glCreateShader` = 创建空白源文件
- `glShaderSource` = 写入代码
- `glCompileShader` = 编译成 .o 文件
- `glCreateProgram` = 创建可执行文件
- `glAttachShader` = 添加 .o 文件
- `glLinkProgram` = 链接成最终程序
- `glUseProgram` = 运行程序

---

## 4. 缓冲对象

### 4.1 VBO (Vertex Buffer Object) - 顶点缓冲对象

**作用：** 在 GPU 显存中存储顶点数据。

```cpp
float vertices[] = {
    -0.5f, -0.5f, 0.0f,  // 顶点 1
     0.0f,  0.5f, 0.0f,  // 顶点 2
     0.5f, -0.5f, 0.0f   // 顶点 3
};

unsigned int VBO;
glGenBuffers(1, &VBO);                 // 生成缓冲区 ID
glBindBuffer(GL_ARRAY_BUFFER, VBO);    // 绑定到 GL_ARRAY_BUFFER 目标
glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);  // 上传数据
```

**GL_STATIC_DRAW 的含义：**

| 标志 | 含义 |
|------|------|
| `GL_STATIC_DRAW` | 数据几乎不变，适合静态模型 |
| `GL_DYNAMIC_DRAW` | 数据经常改变，适合动画 |
| `GL_STREAM_DRAW` | 数据每帧都变，适合粒子系统 |

---

### 4.2 VAO (Vertex Array Object) - 顶点数组对象

**作用：** 记录顶点属性的配置，避免每帧重复设置。

```cpp
unsigned int VAO;
glGenVertexArrays(1, &VAO);  // 生成 VAO
glBindVertexArray(VAO);       // 绑定 VAO

// 以下配置会被 VAO 记住
glBindBuffer(GL_ARRAY_BUFFER, VBO);
glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
glEnableVertexAttribArray(0);
```

**`glVertexAttribPointer` 参数详解：**

```cpp
glVertexAttribPointer(
    0,                  // 属性位置（对应着色器的 location = 0）
    3,                  // 每个属性的分量数（vec3 = 3）
    GL_FLOAT,           // 数据类型
    GL_FALSE,           // 是否归一化
    3 * sizeof(float),  // 步长：相邻顶点的字节间隔
    (void*)0            // 偏移：属性在顶点中的起始位置
);
```

**VAO 的好处：**

```cpp
// 没有 VAO 时，每帧都要这样：
glBindBuffer(GL_ARRAY_BUFFER, VBO);
glVertexAttribPointer(...);
glEnableVertexAttribArray(0);
glDrawArrays(...);

// 有 VAO 后，只需要：
glBindVertexArray(VAO);
glDrawArrays(...);
```

---

### 4.3 VBO 与 VAO 的关系

```
┌─────────────────────────────────────────────────────────┐
│                         VAO                             │
│  ┌─────────────────────────────────────────────────┐   │
│  │ 属性 0: VBO=xxx, size=3, type=FLOAT, stride=12  │   │
│  └─────────────────────────────────────────────────┘   │
│  ┌─────────────────────────────────────────────────┐   │
│  │ 属性 1: (未使用)                                 │   │
│  └─────────────────────────────────────────────────┘   │
│  ...                                                    │
└─────────────────────────────────────────────────────────┘
         │
         │ 引用
         ▼
┌─────────────────────────────────────────────────────────┐
│                         VBO                             │
│  [-0.5, -0.5, 0.0] [0.0, 0.5, 0.0] [0.5, -0.5, 0.0]    │
└─────────────────────────────────────────────────────────┘
```

**记忆口诀：**
- VBO = 数据仓库（存放顶点数据）
- VAO = 说明书（告诉 GPU 如何读取数据）

---

## 5. 渲染循环

### 5.1 双缓冲 (Double Buffering)

**问题：** 如果直接在屏幕上画，用户会看到绘制过程（闪烁）。

**解决方案：** 使用两个缓冲区：
- **前缓冲 (Front Buffer)：** 当前显示在屏幕上
- **后缓冲 (Back Buffer)：** 正在绘制的画面

```cpp
glfwSwapBuffers(window);  // 交换前后缓冲
```

```
时间线：
┌──────────┐     ┌──────────┐     ┌──────────┐
│ 绘制到后  │ ──▶ │ 交换缓冲  │ ──▶ │ 绘制到后  │ ──▶ ...
│ 缓冲      │     │ 显示画面  │     │ 缓冲      │
└──────────┘     └──────────┘     └──────────┘
```

---

### 5.2 事件循环

```cpp
while (!glfwWindowShouldClose(window)) {
    // 1. 清屏
    glClear(GL_COLOR_BUFFER_BIT);
    
    // 2. 绑定着色器和数据
    glUseProgram(shaderProgram);
    glBindVertexArray(VAO);
    
    // 3. 绘制
    glDrawArrays(GL_TRIANGLES, 0, 3);
    
    // 4. 交换缓冲，显示画面
    glfwSwapBuffers(window);
    
    // 5. 处理输入事件
    glfwPollEvents();
}
```

**`glDrawArrays` 参数：**

```cpp
glDrawArrays(
    GL_TRIANGLES,  // 图元类型：三角形
    0,             // 起始索引
    3              // 顶点数量
);
```

**常见图元类型：**

| 类型 | 含义 |
|------|------|
| `GL_POINTS` | 点 |
| `GL_LINES` | 线段（每 2 个顶点一条线） |
| `GL_TRIANGLES` | 三角形（每 3 个顶点一个三角形） |
| `GL_TRIANGLE_STRIP` | 三角形带（共享顶点） |
| `GL_TRIANGLE_FAN` | 三角形扇（共享一个中心点） |

---

## 6. 代码流程图

```
程序启动
    │
    ▼
┌─────────────────┐
│   glfwInit()    │  初始化 GLFW
└────────┬────────┘
         │
         ▼
┌─────────────────┐
│  设置窗口属性    │  OpenGL 版本、Core Profile
└────────┬────────┘
         │
         ▼
┌─────────────────┐
│  创建窗口       │  glfwCreateWindow()
└────────┬────────┘
         │
         ▼
┌─────────────────┐
│  激活上下文     │  glfwMakeContextCurrent()
└────────┬────────┘
         │
         ▼
┌─────────────────┐
│  加载 OpenGL    │  gladLoadGLLoader()
└────────┬────────┘
         │
         ▼
┌─────────────────┐
│  创建 VAO/VBO   │  准备顶点数据
└────────┬────────┘
         │
         ▼
┌─────────────────┐
│  编译着色器     │  创建、编译、链接
└────────┬────────┘
         │
         ▼
┌─────────────────┐
│  渲染循环       │◀─────────┐
│  ├─ 清屏        │          │
│  ├─ 绑定资源    │          │
│  ├─ 绘制        │          │
│  ├─ 交换缓冲    │          │
│  └─ 处理事件    │──────────┘
└────────┬────────┘
         │ 窗口关闭
         ▼
┌─────────────────┐
│  清理资源       │  删除 VAO/VBO/Program
└────────┬────────┘
         │
         ▼
┌─────────────────┐
│  glfwTerminate  │  关闭 GLFW
└────────┬────────┘
         │
         ▼
      程序结束
```

---

## 7. 关键函数速查表

### GLFW 函数

| 函数 | 作用 |
|------|------|
| `glfwInit()` | 初始化 GLFW |
| `glfwWindowHint(hint, value)` | 设置窗口/上下文属性 |
| `glfwCreateWindow(w, h, title, ...)` | 创建窗口 |
| `glfwMakeContextCurrent(window)` | 激活上下文 |
| `glfwSetFramebufferSizeCallback(...)` | 注册窗口大小回调 |
| `glfwWindowShouldClose(window)` | 检查是否应该关闭 |
| `glfwSwapBuffers(window)` | 交换前后缓冲 |
| `glfwPollEvents()` | 处理待处理事件 |
| `glfwTerminate()` | 清理 GLFW 资源 |

### GLAD 函数

| 函数 | 作用 |
|------|------|
| `gladLoadGLLoader(proc)` | 加载所有 OpenGL 函数 |

### OpenGL 缓冲函数

| 函数 | 作用 |
|------|------|
| `glGenVertexArrays(n, &vao)` | 生成 VAO |
| `glGenBuffers(n, &vbo)` | 生成 VBO |
| `glBindVertexArray(vao)` | 绑定 VAO |
| `glBindBuffer(target, vbo)` | 绑定 VBO |
| `glBufferData(target, size, data, usage)` | 上传数据到缓冲 |
| `glVertexAttribPointer(...)` | 配置顶点属性 |
| `glEnableVertexAttribArray(index)` | 启用顶点属性 |
| `glDeleteVertexArrays(n, &vao)` | 删除 VAO |
| `glDeleteBuffers(n, &vbo)` | 删除 VBO |

### OpenGL 着色器函数

| 函数 | 作用 |
|------|------|
| `glCreateShader(type)` | 创建着色器 |
| `glShaderSource(shader, count, &src, len)` | 设置源代码 |
| `glCompileShader(shader)` | 编译着色器 |
| `glCreateProgram()` | 创建程序 |
| `glAttachShader(program, shader)` | 附加着色器 |
| `glLinkProgram(program)` | 链接程序 |
| `glUseProgram(program)` | 使用程序 |
| `glDeleteShader(shader)` | 删除着色器 |
| `glDeleteProgram(program)` | 删除程序 |

### OpenGL 渲染函数

| 函数 | 作用 |
|------|------|
| `glViewport(x, y, w, h)` | 设置视口 |
| `glClearColor(r, g, b, a)` | 设置清屏颜色 |
| `glClear(mask)` | 清屏 |
| `glDrawArrays(mode, first, count)` | 绘制图元 |

---

## 总结

恭喜你完成了 OpenGL 的第一课！你已经学会了：

1. ✅ 使用 GLFW 创建窗口和上下文
2. ✅ 使用 GLAD 加载 OpenGL 函数
3. ✅ 理解 Core Profile 和渲染管线
4. ✅ 编写顶点着色器和片段着色器
5. ✅ 使用 VAO/VBO 管理顶点数据
6. ✅ 实现基本的渲染循环

**下一步学习建议：**

- 添加颜色属性到顶点（每个顶点不同颜色）
- 学习 Uniform 变量（动态改变颜色）
- 添加纹理贴图
- 学习坐标变换（Model-View-Projection 矩阵）

---

*Happy Coding! 🎮*

