# Day02 - Hello Triangle 深入理解

> 本文档对应 [LearnOpenGL - Hello Triangle](https://learnopengl.com/Getting-started/Hello-Triangle) 章节
> 
> 配合 `main.cpp` 中的练习代码阅读，系统掌握 OpenGL 图形渲染管线的核心概念。

---

## 目录

1. [今日学习目标](#1-今日学习目标)
2. [图形渲染管线](#2-图形渲染管线)
3. [顶点输入](#3-顶点输入)
4. [顶点着色器](#4-顶点着色器)
5. [片段着色器](#5-片段着色器)
6. [着色器程序](#6-着色器程序)
7. [链接顶点属性](#7-链接顶点属性)
8. [VAO、VBO、EBO 详解](#8-vaovboebo-详解)
9. [绘制三角形](#9-绘制三角形)
10. [练习题解析](#10-练习题解析)
11. [常见错误与调试](#11-常见错误与调试)
12. [关键函数速查](#12-关键函数速查)

---

## 1. 今日学习目标

通过本章学习，你将掌握：

- [ ] 理解图形渲染管线的各个阶段
- [ ] 编写并编译顶点着色器和片段着色器
- [ ] 使用 VAO/VBO 管理顶点数据
- [ ] 使用 EBO 实现索引绘制
- [ ] 绑定多个 VAO/VBO 绘制多个物体
- [ ] 创建多个着色器程序实现不同效果

---

## 2. 图形渲染管线

### 2.1 什么是渲染管线？

**渲染管线（Graphics Pipeline）** 是将 3D 坐标转换为屏幕上 2D 像素的整个过程。

```
┌─────────────────────────────────────────────────────────────────────────┐
│                        图形渲染管线 (Graphics Pipeline)                  │
├─────────────────────────────────────────────────────────────────────────┤
│                                                                         │
│   ┌──────────┐    ┌──────────┐    ┌──────────┐    ┌──────────┐        │
│   │ 顶点数据  │───▶│ 顶点着色器 │───▶│ 图元装配  │───▶│ 几何着色器 │        │
│   │ (输入)   │    │ (可编程)  │    │ (固定)   │    │ (可选)   │        │
│   └──────────┘    └──────────┘    └──────────┘    └──────────┘        │
│                                                         │              │
│                                                         ▼              │
│   ┌──────────┐    ┌──────────┐    ┌──────────┐    ┌──────────┐        │
│   │ 帧缓冲   │◀───│ 混合测试  │◀───│ 片段着色器 │◀───│ 光栅化   │        │
│   │ (输出)   │    │ (固定)   │    │ (可编程)  │    │ (固定)   │        │
│   └──────────┘    └──────────┘    └──────────┘    └──────────┘        │
│                                                                         │
└─────────────────────────────────────────────────────────────────────────┘
```

### 2.2 管线各阶段说明

| 阶段 | 类型 | 说明 |
|------|------|------|
| **顶点着色器** | 可编程 | 处理每个顶点，进行坐标变换 |
| **图元装配** | 固定 | 将顶点组装成图元（点、线、三角形） |
| **几何着色器** | 可选 | 可以生成新的顶点/图元 |
| **光栅化** | 固定 | 将图元转换为片段（像素候选） |
| **片段着色器** | 可编程 | 计算每个片段的最终颜色 |
| **混合测试** | 固定 | 深度测试、模板测试、混合等 |

### 2.3 标准化设备坐标 (NDC)

顶点着色器输出的坐标必须在 **标准化设备坐标 (Normalized Device Coordinates, NDC)** 范围内：

```
        Y
        ▲
   1.0  │    ┌─────────────┐
        │    │             │
        │    │   可见区域   │
   0.0 ─┼────┼──────┬──────┼──▶ X
        │    │      │      │
        │    │             │
  -1.0  │    └─────────────┘
        │
       -1.0       0.0      1.0
```

- X 轴：-1.0 (左) 到 1.0 (右)
- Y 轴：-1.0 (下) 到 1.0 (上)
- Z 轴：-1.0 (近) 到 1.0 (远)

**超出此范围的坐标会被裁剪掉！**

---

## 3. 顶点输入

### 3.1 定义顶点数据

```cpp
float vertices[] = {
    -0.5f, -0.5f, 0.0f,  // 左下角
     0.5f, -0.5f, 0.0f,  // 右下角
     0.0f,  0.5f, 0.0f   // 顶部
};
```

每个顶点包含 3 个 float 值 (x, y, z)，共 9 个 float。

### 3.2 顶点数据的内存布局

```
vertices 数组在内存中的布局：

┌─────────────────────────────────────────────────────────────┐
│ 顶点 0              │ 顶点 1              │ 顶点 2          │
├─────┬─────┬─────────┼─────┬─────┬────────┼─────┬─────┬─────┤
│ x   │ y   │ z       │ x   │ y   │ z      │ x   │ y   │ z   │
├─────┼─────┼─────────┼─────┼─────┼────────┼─────┼─────┼─────┤
│-0.5 │-0.5 │ 0.0     │ 0.5 │-0.5 │ 0.0    │ 0.0 │ 0.5 │ 0.0 │
└─────┴─────┴─────────┴─────┴─────┴────────┴─────┴─────┴─────┘
│◀─────── 12 字节 ────▶│◀─────── 12 字节 ───▶│◀── 12 字节 ──▶│
│◀────────────────────── 步长 (stride) = 12 ─────────────────▶│
```

---

## 4. 顶点着色器

### 4.1 GLSL 基础语法

```glsl
#version 330 core                    // 版本声明：GLSL 3.30 核心模式
layout (location = 0) in vec3 aPos;  // 输入变量：位置属性，location=0
void main()
{
    gl_Position = vec4(aPos, 1.0);   // 输出：裁剪空间坐标
}
```

### 4.2 关键字详解

| 关键字 | 含义 | 示例 |
|--------|------|------|
| `#version 330 core` | GLSL 版本，330 对应 OpenGL 3.3 | 必须在第一行 |
| `layout (location = n)` | 指定属性位置 | 与 `glVertexAttribPointer` 对应 |
| `in` | 输入变量 | 从 VBO 读取数据 |
| `out` | 输出变量 | 传递给下一阶段 |
| `vec3` | 三维向量 | (x, y, z) |
| `vec4` | 四维向量 | (x, y, z, w) |
| `gl_Position` | 内置输出变量 | 顶点的裁剪空间位置 |

### 4.3 为什么需要 vec4？

```glsl
gl_Position = vec4(aPos, 1.0);  // 为什么是 1.0？
```

第四个分量 `w` 用于**透视除法**：
- `w = 1.0`：普通的 3D 点
- `w ≠ 1.0`：用于透视投影变换

最终屏幕坐标 = (x/w, y/w, z/w)

---

## 5. 片段着色器

### 5.1 基本结构

```glsl
#version 330 core
out vec4 FragColor;              // 输出：片段颜色 (RGBA)
void main()
{
    FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);  // 橙色
}
```

### 5.2 颜色表示

| 分量 | 含义 | 范围 |
|------|------|------|
| R | 红色 | 0.0 ~ 1.0 |
| G | 绿色 | 0.0 ~ 1.0 |
| B | 蓝色 | 0.0 ~ 1.0 |
| A | 透明度 | 0.0 (全透明) ~ 1.0 (不透明) |

### 5.3 常用颜色值

```glsl
vec4(1.0, 0.0, 0.0, 1.0)  // 红色
vec4(0.0, 1.0, 0.0, 1.0)  // 绿色
vec4(0.0, 0.0, 1.0, 1.0)  // 蓝色
vec4(1.0, 1.0, 0.0, 1.0)  // 黄色
vec4(1.0, 0.5, 0.2, 1.0)  // 橙色
vec4(1.0, 1.0, 1.0, 1.0)  // 白色
vec4(0.0, 0.0, 0.0, 1.0)  // 黑色
```

---

## 6. 着色器程序

### 6.1 编译流程

```
┌─────────────┐     ┌─────────────┐     ┌─────────────┐
│ 源代码字符串 │────▶│  着色器对象  │────▶│  着色器程序  │
│ (const char*)│     │ (编译后)    │     │ (链接后)    │
└─────────────┘     └─────────────┘     └─────────────┘
       │                   │                   │
       │ glShaderSource    │ glCompileShader   │ glLinkProgram
       │ glCreateShader    │                   │ glAttachShader
       ▼                   ▼                   ▼
```

### 6.2 完整编译代码

```cpp
// 1. 创建着色器对象
unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);

// 2. 附加源代码
glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);

// 3. 编译着色器
glCompileShader(vertexShader);

// 4. 检查编译错误
int success;
char infoLog[512];
glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
if (!success) {
    glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
    std::cout << "顶点着色器编译失败: " << infoLog << std::endl;
}

// 5. 创建程序对象
unsigned int shaderProgram = glCreateProgram();

// 6. 附加着色器
glAttachShader(shaderProgram, vertexShader);
glAttachShader(shaderProgram, fragmentShader);

// 7. 链接程序
glLinkProgram(shaderProgram);

// 8. 检查链接错误
glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
if (!success) {
    glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
    std::cout << "着色器程序链接失败: " << infoLog << std::endl;
}

// 9. 删除不再需要的着色器对象
glDeleteShader(vertexShader);
glDeleteShader(fragmentShader);

// 10. 使用程序
glUseProgram(shaderProgram);
```

---

## 7. 链接顶点属性

### 7.1 glVertexAttribPointer 详解

```cpp
glVertexAttribPointer(
    0,                  // 属性位置 (对应 layout location)
    3,                  // 属性分量数 (vec3 = 3)
    GL_FLOAT,           // 数据类型
    GL_FALSE,           // 是否归一化
    3 * sizeof(float),  // 步长 (stride)
    (void*)0            // 偏移量 (offset)
);
```

### 7.2 参数详解图示

```
顶点数据内存布局：

位置属性 (location = 0)
    │
    ▼
┌─────────────────────────────────────────────────────────┐
│ x0  │ y0  │ z0  │ x1  │ y1  │ z1  │ x2  │ y2  │ z2  │
└─────────────────────────────────────────────────────────┘
│◀─ 偏移 0 ──▶│
│◀────── size = 3 ─────▶│
│◀─────────── stride = 12 字节 ──────────▶│
```

### 7.3 多属性顶点数据

如果顶点包含位置和颜色：

```cpp
float vertices[] = {
    // 位置              // 颜色
    -0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,  // 顶点0
     0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,  // 顶点1
     0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f   // 顶点2
};

// 位置属性
glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
glEnableVertexAttribArray(0);

// 颜色属性
glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
glEnableVertexAttribArray(1);
```

```
多属性内存布局：

┌─────────────────────────────────────────────────────────────────────────┐
│ x0  │ y0  │ z0  │ r0  │ g0  │ b0  │ x1  │ y1  │ z1  │ r1  │ g1  │ b1  │
└─────────────────────────────────────────────────────────────────────────┘
│◀── 位置 ──▶│◀── 颜色 ──▶│
│◀── offset=0 ──▶│◀── offset=12 ──▶│
│◀────────────────── stride = 24 字节 ──────────────────▶│
```

---

## 8. VAO、VBO、EBO 详解

### 8.1 三者关系

```
┌─────────────────────────────────────────────────────────────────────────┐
│                              VAO (顶点数组对象)                          │
│  ┌───────────────────────────────────────────────────────────────────┐ │
│  │ 属性 0: VBO=xxx, size=3, type=FLOAT, stride=12, offset=0         │ │
│  │ 属性 1: VBO=yyy, size=3, type=FLOAT, stride=12, offset=0         │ │
│  │ EBO 绑定: zzz                                                     │ │
│  └───────────────────────────────────────────────────────────────────┘ │
└─────────────────────────────────────────────────────────────────────────┘
         │                           │
         │ 引用                      │ 引用
         ▼                           ▼
┌─────────────────────┐     ┌─────────────────────┐
│        VBO          │     │        EBO          │
│  顶点数据 (位置等)   │     │  索引数据 (整数)    │
│ [-0.5,-0.5,0] ...   │     │ [0, 1, 2, 1, 2, 3]  │
└─────────────────────┘     └─────────────────────┘
```

### 8.2 VAO (Vertex Array Object)

**作用：** 记录顶点属性配置，避免每帧重复设置。

```cpp
// 生成并绑定 VAO
unsigned int VAO;
glGenVertexArrays(1, &VAO);
glBindVertexArray(VAO);

// 此后的 VBO 绑定和属性配置都会被 VAO 记住
```

### 8.3 VBO (Vertex Buffer Object)

**作用：** 在 GPU 显存中存储顶点数据。

```cpp
// 生成并绑定 VBO
unsigned int VBO;
glGenBuffers(1, &VBO);
glBindBuffer(GL_ARRAY_BUFFER, VBO);

// 上传数据
glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
```

**glBufferData 使用提示：**

| 标志 | 含义 | 适用场景 |
|------|------|----------|
| `GL_STATIC_DRAW` | 数据几乎不变 | 静态模型 |
| `GL_DYNAMIC_DRAW` | 数据经常改变 | 动画、粒子 |
| `GL_STREAM_DRAW` | 数据每帧都变 | 实时数据流 |

### 8.4 EBO (Element Buffer Object)

**作用：** 存储顶点索引，复用顶点数据。

**没有 EBO 时（绘制矩形需要 6 个顶点）：**

```cpp
float vertices[] = {
    // 第一个三角形
     0.5f,  0.5f, 0.0f,  // 右上
     0.5f, -0.5f, 0.0f,  // 右下
    -0.5f,  0.5f, 0.0f,  // 左上
    // 第二个三角形
     0.5f, -0.5f, 0.0f,  // 右下 (重复!)
    -0.5f, -0.5f, 0.0f,  // 左下
    -0.5f,  0.5f, 0.0f   // 左上 (重复!)
};
```

**使用 EBO 时（只需 4 个顶点）：**

```cpp
float vertices[] = {
     0.5f,  0.5f, 0.0f,  // 索引 0: 右上
     0.5f, -0.5f, 0.0f,  // 索引 1: 右下
    -0.5f, -0.5f, 0.0f,  // 索引 2: 左下
    -0.5f,  0.5f, 0.0f   // 索引 3: 左上
};

unsigned int indices[] = {
    0, 1, 3,  // 第一个三角形
    1, 2, 3   // 第二个三角形
};
```

**EBO 使用代码：**

```cpp
unsigned int EBO;
glGenBuffers(1, &EBO);
glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

// 绘制时使用 glDrawElements
glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
```

### 8.5 VAO 配置的正确顺序

```cpp
// 1. 绑定 VAO
glBindVertexArray(VAO);

// 2. 绑定并设置 VBO
glBindBuffer(GL_ARRAY_BUFFER, VBO);
glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

// 3. 绑定并设置 EBO (如果使用)
glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

// 4. 配置顶点属性
glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
glEnableVertexAttribArray(0);

// 5. 解绑 VAO (可选，防止误操作)
glBindVertexArray(0);
```

---

## 9. 绘制三角形

### 9.1 glDrawArrays

```cpp
glDrawArrays(
    GL_TRIANGLES,  // 图元类型
    0,             // 起始索引
    3              // 顶点数量
);
```

### 9.2 glDrawElements

```cpp
glDrawElements(
    GL_TRIANGLES,      // 图元类型
    6,                 // 索引数量
    GL_UNSIGNED_INT,   // 索引数据类型
    0                  // 索引数组偏移
);
```

### 9.3 图元类型

| 类型 | 说明 | 图示 |
|------|------|------|
| `GL_POINTS` | 点 | • • • |
| `GL_LINES` | 线段 | —— —— |
| `GL_LINE_STRIP` | 连续线 | ———— |
| `GL_LINE_LOOP` | 闭合线 | ◇ |
| `GL_TRIANGLES` | 独立三角形 | △ △ |
| `GL_TRIANGLE_STRIP` | 三角形带 | ▽△▽ |
| `GL_TRIANGLE_FAN` | 三角形扇 | ⌔ |

### 9.4 线框模式

```cpp
// 开启线框模式
glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

// 恢复填充模式
glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
```

---

## 10. 练习题解析

### 练习 1：绘制两个相连的三角形

**目标：** 使用 `glDrawArrays` 绘制两个彼此相连的三角形。

**关键点：**
- 6 个顶点存入一个 VBO
- 调用 `glDrawArrays(GL_TRIANGLES, 0, 6)`

```cpp
float vertices[] = {
    // 第一个三角形
    -0.9f, -0.5f, 0.0f,
    -0.0f, -0.5f, 0.0f,
    -0.45f, 0.5f, 0.0f,
    // 第二个三角形
     0.0f, -0.5f, 0.0f,
     0.9f, -0.5f, 0.0f,
     0.45f, 0.5f, 0.0f
};

glDrawArrays(GL_TRIANGLES, 0, 6);  // 绘制 6 个顶点
```

### 练习 2：使用不同的 VAO 和 VBO

**目标：** 两个三角形使用独立的 VAO 和 VBO。

**关键点：**
- 创建两套 VAO/VBO
- 分别配置和绘制

```cpp
unsigned int VAOs[2], VBOs[2];
glGenVertexArrays(2, VAOs);
glGenBuffers(2, VBOs);

// 配置第一个三角形
glBindVertexArray(VAOs[0]);
glBindBuffer(GL_ARRAY_BUFFER, VBOs[0]);
glBufferData(...);
glVertexAttribPointer(...);
glEnableVertexAttribArray(0);

// 配置第二个三角形
glBindVertexArray(VAOs[1]);
glBindBuffer(GL_ARRAY_BUFFER, VBOs[1]);
glBufferData(...);
glVertexAttribPointer(...);
glEnableVertexAttribArray(0);

// 绘制
glBindVertexArray(VAOs[0]);
glDrawArrays(GL_TRIANGLES, 0, 3);

glBindVertexArray(VAOs[1]);
glDrawArrays(GL_TRIANGLES, 0, 3);
```

### 练习 3：使用不同的着色器程序

**目标：** 两个三角形使用不同颜色（不同的片段着色器）。

**关键点：**
- 创建两个着色器程序
- 可以共享顶点着色器
- 使用不同的片段着色器

```cpp
// 橙色片段着色器
const char* fragmentShaderSourceOrange = R"(
#version 330 core
out vec4 FragColor;
void main() {
    FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);
})";

// 黄色片段着色器
const char* fragmentShaderSourceYellow = R"(
#version 330 core
out vec4 FragColor;
void main() {
    FragColor = vec4(1.0f, 1.0f, 0.0f, 1.0f);
})";

// 创建两个程序
unsigned int shaderProgramOrange = glCreateProgram();
glAttachShader(shaderProgramOrange, vertexShader);
glAttachShader(shaderProgramOrange, fragmentShaderOrange);
glLinkProgram(shaderProgramOrange);

unsigned int shaderProgramYellow = glCreateProgram();
glAttachShader(shaderProgramYellow, vertexShader);
glAttachShader(shaderProgramYellow, fragmentShaderYellow);
glLinkProgram(shaderProgramYellow);

// 绘制
glUseProgram(shaderProgramOrange);
glBindVertexArray(VAOs[0]);
glDrawArrays(GL_TRIANGLES, 0, 3);

glUseProgram(shaderProgramYellow);
glBindVertexArray(VAOs[1]);
glDrawArrays(GL_TRIANGLES, 0, 3);
```

### 练习 3 扩展：使用 EBO

**目标：** 在练习 3 基础上改用 EBO。

```cpp
unsigned int VBOs[2], VAOs[2], EBOs[2];
glGenVertexArrays(2, VAOs);
glGenBuffers(2, VBOs);
glGenBuffers(2, EBOs);

// 配置第一个三角形
glBindVertexArray(VAOs[0]);
glBindBuffer(GL_ARRAY_BUFFER, VBOs[0]);
glBufferData(GL_ARRAY_BUFFER, sizeof(firstVertices), firstVertices, GL_STATIC_DRAW);
glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOs[0]);
glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(firstIndices), firstIndices, GL_STATIC_DRAW);
glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
glEnableVertexAttribArray(0);

// 绘制
glUseProgram(shaderProgramOrange);
glBindVertexArray(VAOs[0]);
glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);  // 使用 glDrawElements
```

---

## 11. 常见错误与调试

### 11.1 常见错误

| 错误现象 | 可能原因 | 解决方案 |
|----------|----------|----------|
| 窗口全黑 | 没有调用 `glfwMakeContextCurrent` | 在 GLAD 初始化前调用 |
| 窗口全黑 | 使用了固定管线函数 | macOS Core Profile 不支持 glBegin/glEnd |
| 三角形不显示 | VAO 没有正确配置 | 检查绑定顺序 |
| 着色器编译失败 | GLSL 语法错误 | 检查错误日志 |
| 颜色不对 | 使用了错误的着色器程序 | 检查 glAttachShader 参数 |

### 11.2 调试技巧

**1. 检查着色器编译状态：**

```cpp
int success;
char infoLog[512];
glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
if (!success) {
    glGetShaderInfoLog(shader, 512, nullptr, infoLog);
    std::cout << "错误: " << infoLog << std::endl;
}
```

**2. 检查程序链接状态：**

```cpp
glGetProgramiv(program, GL_LINK_STATUS, &success);
if (!success) {
    glGetProgramInfoLog(program, 512, nullptr, infoLog);
    std::cout << "链接错误: " << infoLog << std::endl;
}
```

**3. 检查 OpenGL 版本：**

```cpp
std::cout << "OpenGL 版本: " << glGetString(GL_VERSION) << std::endl;
std::cout << "GLSL 版本: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
```

---

## 12. 关键函数速查

### 着色器相关

| 函数 | 作用 |
|------|------|
| `glCreateShader(type)` | 创建着色器对象 |
| `glShaderSource(shader, count, &src, len)` | 设置源代码 |
| `glCompileShader(shader)` | 编译着色器 |
| `glGetShaderiv(shader, pname, &params)` | 获取着色器参数 |
| `glGetShaderInfoLog(shader, maxLen, &len, log)` | 获取编译日志 |
| `glDeleteShader(shader)` | 删除着色器 |

### 程序相关

| 函数 | 作用 |
|------|------|
| `glCreateProgram()` | 创建程序对象 |
| `glAttachShader(program, shader)` | 附加着色器 |
| `glLinkProgram(program)` | 链接程序 |
| `glUseProgram(program)` | 使用程序 |
| `glGetProgramiv(program, pname, &params)` | 获取程序参数 |
| `glGetProgramInfoLog(program, maxLen, &len, log)` | 获取链接日志 |
| `glDeleteProgram(program)` | 删除程序 |

### 缓冲相关

| 函数 | 作用 |
|------|------|
| `glGenBuffers(n, &buffers)` | 生成缓冲对象 |
| `glBindBuffer(target, buffer)` | 绑定缓冲 |
| `glBufferData(target, size, data, usage)` | 上传数据 |
| `glDeleteBuffers(n, &buffers)` | 删除缓冲 |

### VAO 相关

| 函数 | 作用 |
|------|------|
| `glGenVertexArrays(n, &arrays)` | 生成 VAO |
| `glBindVertexArray(array)` | 绑定 VAO |
| `glVertexAttribPointer(...)` | 配置顶点属性 |
| `glEnableVertexAttribArray(index)` | 启用属性 |
| `glDeleteVertexArrays(n, &arrays)` | 删除 VAO |

### 绘制相关

| 函数 | 作用 |
|------|------|
| `glDrawArrays(mode, first, count)` | 按顺序绘制 |
| `glDrawElements(mode, count, type, indices)` | 按索引绘制 |
| `glPolygonMode(face, mode)` | 设置多边形模式 |

---

## 总结

恭喜完成 Day02 的学习！你已经掌握了：

1. ✅ 图形渲染管线的工作流程
2. ✅ GLSL 着色器语言基础
3. ✅ 着色器的编译和链接
4. ✅ VAO/VBO/EBO 的使用
5. ✅ 绘制多个物体的方法
6. ✅ 使用多个着色器程序

**下一步学习建议：**

- 学习 [Shaders](https://learnopengl.com/Getting-started/Shaders) 章节，深入理解 GLSL
- 学习 Uniform 变量，实现动态效果
- 学习 [Textures](https://learnopengl.com/Getting-started/Textures) 章节，给三角形贴图

---

## 参考资料

- [LearnOpenGL - Hello Triangle](https://learnopengl.com/Getting-started/Hello-Triangle)
- [LearnOpenGL 中文版](https://learnopengl-cn.github.io/)
- [OpenGL Reference Pages](https://www.khronos.org/registry/OpenGL-Refpages/gl4/)

---

*Happy Coding!*

