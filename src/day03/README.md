# Day03 - GLSL 着色器语言深入

> 本文档对应 [LearnOpenGL - Shaders](https://learnopengl.com/Getting-started/Shaders) 章节
> 
> 深入学习 GLSL (OpenGL Shading Language) 着色器语言，掌握着色器编程的核心技术。

---

## 目录

1. [今日学习目标](#1-今日学习目标)
2. [GLSL 语言基础](#2-glsl-语言基础)
3. [数据类型详解](#3-数据类型详解)
4. [向量与矩阵运算](#4-向量与矩阵运算)
5. [输入与输出](#5-输入与输出)
6. [Uniform 变量](#6-uniform-变量)
7. [顶点属性扩展](#7-顶点属性扩展)
8. [着色器类封装](#8-着色器类封装)
9. [练习题](#9-练习题)
10. [常见错误与调试](#10-常见错误与调试)
11. [GLSL 内置函数速查](#11-glsl-内置函数速查)

---

## 1. 今日学习目标

通过本章学习，你将掌握：

- [ ] GLSL 语言的基本语法和结构
- [ ] 向量、矩阵等数据类型的使用
- [ ] 顶点着色器和片段着色器之间的数据传递
- [ ] Uniform 变量的概念和使用
- [ ] 多顶点属性（位置 + 颜色）的配置
- [ ] 封装一个可复用的 Shader 类

---

## 2. GLSL 语言基础

### 2.1 什么是 GLSL？

**GLSL (OpenGL Shading Language)** 是专门为 OpenGL 设计的着色器编程语言，语法类似 C 语言。

```glsl
#version 330 core  // 版本声明，必须在第一行

// 输入变量
in vec3 aPos;

// 输出变量
out vec4 vertexColor;

// Uniform 变量
uniform float time;

// 主函数
void main()
{
    gl_Position = vec4(aPos, 1.0);
    vertexColor = vec4(0.5, 0.0, 0.0, 1.0);
}
```

### 2.2 着色器结构

```
┌─────────────────────────────────────────────────────────────┐
│                     GLSL 着色器结构                          │
├─────────────────────────────────────────────────────────────┤
│  #version 330 core              // 1. 版本声明               │
│                                                             │
│  // 2. 输入变量                                              │
│  in type inputName;                                         │
│                                                             │
│  // 3. 输出变量                                              │
│  out type outputName;                                       │
│                                                             │
│  // 4. Uniform 变量                                         │
│  uniform type uniformName;                                  │
│                                                             │
│  // 5. 主函数                                                │
│  void main()                                                │
│  {                                                          │
│      // 着色器逻辑                                           │
│  }                                                          │
└─────────────────────────────────────────────────────────────┘
```

### 2.3 版本对应关系

| GLSL 版本 | OpenGL 版本 | 声明 |
|-----------|-------------|------|
| 1.10 | 2.0 | `#version 110` |
| 1.20 | 2.1 | `#version 120` |
| 1.30 | 3.0 | `#version 130` |
| 1.40 | 3.1 | `#version 140` |
| 1.50 | 3.2 | `#version 150` |
| 3.30 | 3.3 | `#version 330` |
| 4.00 | 4.0 | `#version 400` |
| 4.10 | 4.1 | `#version 410` |

> **注意：** macOS 最高支持 OpenGL 4.1，对应 GLSL 4.10。但通常使用 3.30 即可。

---

## 3. 数据类型详解

### 3.1 基本类型

| 类型 | 说明 | 示例 |
|------|------|------|
| `bool` | 布尔值 | `bool flag = true;` |
| `int` | 有符号整数 | `int count = 10;` |
| `uint` | 无符号整数 | `uint index = 5u;` |
| `float` | 单精度浮点数 | `float value = 3.14;` |
| `double` | 双精度浮点数 | `double precise = 3.14159265;` |

### 3.2 向量类型

向量是 GLSL 中最常用的数据类型：

| 类型 | 分量数 | 说明 |
|------|--------|------|
| `vec2` | 2 | 二维浮点向量 |
| `vec3` | 3 | 三维浮点向量 |
| `vec4` | 4 | 四维浮点向量 |
| `ivec2/3/4` | 2/3/4 | 整数向量 |
| `uvec2/3/4` | 2/3/4 | 无符号整数向量 |
| `bvec2/3/4` | 2/3/4 | 布尔向量 |
| `dvec2/3/4` | 2/3/4 | 双精度向量 |

### 3.3 向量分量访问

GLSL 提供了多种方式访问向量分量：

```glsl
vec4 v = vec4(1.0, 2.0, 3.0, 4.0);

// 方式 1：xyzw（位置）
float x = v.x;  // 1.0
float y = v.y;  // 2.0
float z = v.z;  // 3.0
float w = v.w;  // 4.0

// 方式 2：rgba（颜色）
float r = v.r;  // 1.0
float g = v.g;  // 2.0
float b = v.b;  // 3.0
float a = v.a;  // 4.0

// 方式 3：stpq（纹理坐标）
float s = v.s;  // 1.0
float t = v.t;  // 2.0
float p = v.p;  // 3.0
float q = v.q;  // 4.0
```

### 3.4 向量重组 (Swizzling)

GLSL 支持灵活的向量分量重组：

```glsl
vec4 v = vec4(1.0, 2.0, 3.0, 4.0);

vec2 xy = v.xy;           // (1.0, 2.0)
vec3 xyz = v.xyz;         // (1.0, 2.0, 3.0)
vec3 zyx = v.zyx;         // (3.0, 2.0, 1.0) 反转顺序
vec4 xxxx = v.xxxx;       // (1.0, 1.0, 1.0, 1.0) 重复分量
vec3 rgb = v.rgb;         // (1.0, 2.0, 3.0)
vec2 rg = v.rg;           // (1.0, 2.0)

// 用于赋值
vec4 color;
color.rgb = vec3(1.0, 0.5, 0.2);  // 只设置 rgb
color.a = 1.0;                    // 单独设置 alpha
```

### 3.5 向量构造

```glsl
// 直接构造
vec3 v1 = vec3(1.0, 2.0, 3.0);

// 标量扩展
vec3 v2 = vec3(1.0);  // (1.0, 1.0, 1.0)

// 组合构造
vec2 v2d = vec2(1.0, 2.0);
vec3 v3 = vec3(v2d, 3.0);  // (1.0, 2.0, 3.0)

vec4 v4 = vec4(v3, 4.0);   // (1.0, 2.0, 3.0, 4.0)
vec4 v5 = vec4(v2d, v2d);  // (1.0, 2.0, 1.0, 2.0)
```

### 3.6 矩阵类型

| 类型 | 说明 |
|------|------|
| `mat2` | 2x2 矩阵 |
| `mat3` | 3x3 矩阵 |
| `mat4` | 4x4 矩阵 |
| `mat2x3` | 2 列 3 行矩阵 |
| `mat3x4` | 3 列 4 行矩阵 |

```glsl
// 单位矩阵
mat4 identity = mat4(1.0);

// 按列构造
mat2 m = mat2(
    1.0, 2.0,  // 第一列
    3.0, 4.0   // 第二列
);

// 访问元素
float element = m[0][1];  // 第 0 列，第 1 行 = 2.0
vec2 column0 = m[0];      // 第 0 列 = (1.0, 2.0)
```

---

## 4. 向量与矩阵运算

### 4.1 向量运算

```glsl
vec3 a = vec3(1.0, 2.0, 3.0);
vec3 b = vec3(4.0, 5.0, 6.0);

// 加减乘除（逐分量）
vec3 sum = a + b;      // (5.0, 7.0, 9.0)
vec3 diff = a - b;     // (-3.0, -3.0, -3.0)
vec3 prod = a * b;     // (4.0, 10.0, 18.0)
vec3 quot = a / b;     // (0.25, 0.4, 0.5)

// 标量运算
vec3 scaled = a * 2.0; // (2.0, 4.0, 6.0)

// 点积
float d = dot(a, b);   // 1*4 + 2*5 + 3*6 = 32.0

// 叉积（仅 vec3）
vec3 c = cross(a, b);  // (-3.0, 6.0, -3.0)

// 长度
float len = length(a); // sqrt(1+4+9) = 3.74...

// 归一化
vec3 n = normalize(a); // 单位向量

// 距离
float dist = distance(a, b);

// 反射
vec3 reflected = reflect(incident, normal);
```

### 4.2 矩阵运算

```glsl
mat4 m1 = mat4(1.0);
mat4 m2 = mat4(2.0);
vec4 v = vec4(1.0, 2.0, 3.0, 1.0);

// 矩阵乘法
mat4 result = m1 * m2;

// 矩阵与向量相乘
vec4 transformed = m1 * v;

// 转置
mat4 transposed = transpose(m1);

// 逆矩阵
mat4 inverted = inverse(m1);

// 行列式
float det = determinant(mat3(m1));
```

---

## 5. 输入与输出

### 5.1 顶点着色器到片段着色器的数据传递

```
┌─────────────────────────────────────────────────────────────────────────┐
│                         数据传递流程                                     │
├─────────────────────────────────────────────────────────────────────────┤
│                                                                         │
│   ┌──────────────┐                          ┌──────────────┐           │
│   │  顶点着色器   │                          │  片段着色器   │           │
│   │              │                          │              │           │
│   │ in vec3 aPos │◀── VBO 数据               │              │           │
│   │ in vec3 aCol │◀── VBO 数据               │              │           │
│   │              │                          │              │           │
│   │ out vec3 col │─────────────────────────▶│ in vec3 col  │           │
│   │              │     (插值后传递)          │              │           │
│   │              │                          │ out vec4 Frag│──▶ 屏幕   │
│   └──────────────┘                          └──────────────┘           │
│                                                                         │
└─────────────────────────────────────────────────────────────────────────┘
```

### 5.2 顶点着色器

```glsl
#version 330 core

// 输入：从 VBO 读取
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;

// 输出：传递给片段着色器
out vec3 ourColor;

void main()
{
    gl_Position = vec4(aPos, 1.0);
    ourColor = aColor;  // 传递颜色到片段着色器
}
```

### 5.3 片段着色器

```glsl
#version 330 core

// 输入：从顶点着色器接收（名称必须匹配！）
in vec3 ourColor;

// 输出：最终颜色
out vec4 FragColor;

void main()
{
    FragColor = vec4(ourColor, 1.0);
}
```

### 5.4 插值 (Interpolation)

片段着色器接收的值是**插值后**的结果：

```
顶点 0 (红色)                    顶点 1 (绿色)
     ●─────────────────────────────●
      \                           /
       \    插值区域             /
        \   (渐变色)           /
         \                   /
          \                 /
           \               /
            \             /
             \           /
              \         /
               \       /
                \     /
                 \   /
                  \ /
                   ●
              顶点 2 (蓝色)
```

三角形内部的每个像素颜色都是三个顶点颜色的加权平均。

---

## 6. Uniform 变量

### 6.1 什么是 Uniform？

**Uniform** 是一种从 CPU 向 GPU 着色器传递数据的方式：

- 全局变量：所有着色器实例共享同一个值
- 只读：着色器中不能修改
- 持久：在调用 `glUniform*` 之前保持不变

### 6.2 在着色器中声明

```glsl
#version 330 core

uniform vec4 ourColor;  // 声明 uniform 变量
uniform float time;
uniform mat4 transform;

out vec4 FragColor;

void main()
{
    FragColor = ourColor;
}
```

### 6.3 在 C++ 中设置 Uniform

```cpp
// 1. 获取 uniform 变量的位置
int colorLocation = glGetUniformLocation(shaderProgram, "ourColor");

// 2. 设置 uniform 值（必须先激活着色器程序！）
glUseProgram(shaderProgram);
glUniform4f(colorLocation, 0.0f, greenValue, 0.0f, 1.0f);
```

### 6.4 glUniform* 函数族

| 函数 | 说明 |
|------|------|
| `glUniform1f(loc, v)` | 设置 1 个 float |
| `glUniform2f(loc, v0, v1)` | 设置 2 个 float |
| `glUniform3f(loc, v0, v1, v2)` | 设置 3 个 float |
| `glUniform4f(loc, v0, v1, v2, v3)` | 设置 4 个 float |
| `glUniform1i(loc, v)` | 设置 1 个 int |
| `glUniform1fv(loc, count, &v)` | 设置 float 数组 |
| `glUniform3fv(loc, count, &v)` | 设置 vec3 数组 |
| `glUniformMatrix4fv(loc, count, transpose, &m)` | 设置 mat4 |

### 6.5 动态颜色示例

```cpp
// 渲染循环中
while (!glfwWindowShouldClose(window))
{
    // 获取时间
    float timeValue = glfwGetTime();
    
    // 计算颜色（0.0 ~ 1.0 之间变化）
    float greenValue = (sin(timeValue) / 2.0f) + 0.5f;
    
    // 设置 uniform
    int colorLocation = glGetUniformLocation(shaderProgram, "ourColor");
    glUseProgram(shaderProgram);
    glUniform4f(colorLocation, 0.0f, greenValue, 0.0f, 1.0f);
    
    // 绑定 VAO 并绘制
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    
    // ...
}
```

---

## 7. 顶点属性扩展

### 7.1 添加颜色属性

现在我们让每个顶点都有自己的颜色：

```cpp
// 顶点数据：位置 (xyz) + 颜色 (rgb)
float vertices[] = {
    // 位置              // 颜色
     0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,  // 右下 - 红色
    -0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,  // 左下 - 绿色
     0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f   // 顶部 - 蓝色
};
```

### 7.2 内存布局

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                              顶点数据内存布局                                │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                             │
│ 属性 0 (位置)        属性 1 (颜色)        属性 0 (位置)        属性 1 (颜色) │
│ ┌─────────────────┬─────────────────┬─────────────────┬─────────────────┐  │
│ │  x  │  y  │  z  │  r  │  g  │  b  │  x  │  y  │  z  │  r  │  g  │  b  │  │
│ └─────────────────┴─────────────────┴─────────────────┴─────────────────┘  │
│ │◀─ 12 字节 ─────▶│◀─ 12 字节 ─────▶│                                       │
│ │◀─ offset = 0 ──▶│◀─ offset = 12 ─▶│                                       │
│ │◀────────────────── stride = 24 字节 ──────────────────▶│                  │
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘
```

### 7.3 配置顶点属性

```cpp
// 位置属性 (location = 0)
glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
glEnableVertexAttribArray(0);

// 颜色属性 (location = 1)
glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
glEnableVertexAttribArray(1);
```

### 7.4 更新着色器

**顶点着色器：**

```glsl
#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;

out vec3 ourColor;

void main()
{
    gl_Position = vec4(aPos, 1.0);
    ourColor = aColor;
}
```

**片段着色器：**

```glsl
#version 330 core

in vec3 ourColor;
out vec4 FragColor;

void main()
{
    FragColor = vec4(ourColor, 1.0);
}
```

### 7.5 效果展示

```
                    蓝色 (0, 0, 1)
                         ▲
                        /|\
                       / | \
                      /  |  \
                     /   |   \
                    /    |    \
                   /     |     \
                  /  渐变效果   \
                 /       |       \
                /        |        \
               /         |         \
              /          |          \
             ▼───────────┴───────────▼
        红色 (1, 0, 0)           绿色 (0, 1, 0)
```

三角形内部会自动产生 RGB 渐变效果！

---

## 8. 着色器类封装

### 8.1 为什么要封装？

每次都写编译、链接、错误检查的代码太繁琐，封装成类更方便：

```cpp
// 使用前
unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
glCompileShader(vertexShader);
// ... 检查错误 ...
// ... 创建片段着色器 ...
// ... 创建程序 ...
// ... 链接 ...
// ... 检查错误 ...

// 封装后
Shader ourShader("vertex.glsl", "fragment.glsl");
ourShader.use();
ourShader.setFloat("time", glfwGetTime());
```

### 8.2 Shader 类头文件

```cpp
// shader.h
#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class Shader
{
public:
    // 程序 ID
    unsigned int ID;

    // 构造函数：从文件读取并编译着色器
    Shader(const char* vertexPath, const char* fragmentPath);

    // 激活着色器程序
    void use();

    // uniform 工具函数
    void setBool(const std::string &name, bool value) const;
    void setInt(const std::string &name, int value) const;
    void setFloat(const std::string &name, float value) const;
    void setVec3(const std::string &name, float x, float y, float z) const;
    void setVec4(const std::string &name, float x, float y, float z, float w) const;
    void setMat4(const std::string &name, const float* value) const;

private:
    // 检查编译/链接错误
    void checkCompileErrors(unsigned int shader, std::string type);
};

#endif
```

### 8.3 Shader 类实现

```cpp
// shader.cpp
#include "shader.h"

Shader::Shader(const char* vertexPath, const char* fragmentPath)
{
    // 1. 从文件读取着色器代码
    std::string vertexCode;
    std::string fragmentCode;
    std::ifstream vShaderFile;
    std::ifstream fShaderFile;

    // 确保 ifstream 对象可以抛出异常
    vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    try
    {
        // 打开文件
        vShaderFile.open(vertexPath);
        fShaderFile.open(fragmentPath);
        std::stringstream vShaderStream, fShaderStream;

        // 读取文件内容到流中
        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();

        // 关闭文件
        vShaderFile.close();
        fShaderFile.close();

        // 转换为字符串
        vertexCode = vShaderStream.str();
        fragmentCode = fShaderStream.str();
    }
    catch (std::ifstream::failure& e)
    {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ: " << e.what() << std::endl;
    }

    const char* vShaderCode = vertexCode.c_str();
    const char* fShaderCode = fragmentCode.c_str();

    // 2. 编译着色器
    unsigned int vertex, fragment;

    // 顶点着色器
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, NULL);
    glCompileShader(vertex);
    checkCompileErrors(vertex, "VERTEX");

    // 片段着色器
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, NULL);
    glCompileShader(fragment);
    checkCompileErrors(fragment, "FRAGMENT");

    // 3. 着色器程序
    ID = glCreateProgram();
    glAttachShader(ID, vertex);
    glAttachShader(ID, fragment);
    glLinkProgram(ID);
    checkCompileErrors(ID, "PROGRAM");

    // 4. 删除着色器对象
    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

void Shader::use()
{
    glUseProgram(ID);
}

void Shader::setBool(const std::string &name, bool value) const
{
    glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
}

void Shader::setInt(const std::string &name, int value) const
{
    glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setFloat(const std::string &name, float value) const
{
    glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setVec3(const std::string &name, float x, float y, float z) const
{
    glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
}

void Shader::setVec4(const std::string &name, float x, float y, float z, float w) const
{
    glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w);
}

void Shader::setMat4(const std::string &name, const float* value) const
{
    glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, value);
}

void Shader::checkCompileErrors(unsigned int shader, std::string type)
{
    int success;
    char infoLog[1024];

    if (type != "PROGRAM")
    {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(shader, 1024, NULL, infoLog);
            std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n"
                      << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
        }
    }
    else
    {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(shader, 1024, NULL, infoLog);
            std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n"
                      << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
        }
    }
}
```

### 8.4 使用 Shader 类

```cpp
// 创建着色器
Shader ourShader("shaders/vertex.glsl", "shaders/fragment.glsl");

// 渲染循环
while (!glfwWindowShouldClose(window))
{
    // 清屏
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // 激活着色器
    ourShader.use();

    // 设置 uniform
    ourShader.setFloat("time", glfwGetTime());

    // 绘制
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    glfwSwapBuffers(window);
    glfwPollEvents();
}
```

---

## 9. 练习题

### 练习 1：调整顶点着色器使三角形上下颠倒

**提示：** 修改顶点着色器中的 `gl_Position`

```glsl
void main()
{
    gl_Position = vec4(aPos.x, -aPos.y, aPos.z, 1.0);  // y 取反
}
```

### 练习 2：使用 uniform 定义一个水平偏移量

**顶点着色器：**

```glsl
#version 330 core

layout (location = 0) in vec3 aPos;

uniform float xOffset;

void main()
{
    gl_Position = vec4(aPos.x + xOffset, aPos.y, aPos.z, 1.0);
}
```

**C++ 代码：**

```cpp
ourShader.use();
ourShader.setFloat("xOffset", 0.5f);
```

### 练习 3：使用 out 关键字把顶点位置输出到片段着色器

**顶点着色器：**

```glsl
#version 330 core

layout (location = 0) in vec3 aPos;

out vec3 vertexPos;

void main()
{
    gl_Position = vec4(aPos, 1.0);
    vertexPos = aPos;
}
```

**片段着色器：**

```glsl
#version 330 core

in vec3 vertexPos;
out vec4 FragColor;

void main()
{
    // 把位置作为颜色（需要从 [-1,1] 映射到 [0,1]）
    FragColor = vec4((vertexPos + 1.0) / 2.0, 1.0);
}
```

---

## 10. 常见错误与调试

### 10.1 常见错误

| 错误现象 | 可能原因 | 解决方案 |
|----------|----------|----------|
| `uniform location = -1` | uniform 名称拼写错误 | 检查名称是否完全匹配 |
| `uniform location = -1` | uniform 未使用被优化掉 | 确保在着色器中使用了该 uniform |
| 颜色不变化 | 忘记调用 `glUseProgram` | 设置 uniform 前必须激活程序 |
| 片段着色器收不到数据 | 变量名不匹配 | 顶点着色器的 `out` 和片段着色器的 `in` 名称必须相同 |
| 编译错误 | GLSL 语法错误 | 检查错误日志 |

### 10.2 调试技巧

**1. 检查 uniform 位置：**

```cpp
int location = glGetUniformLocation(program, "uniformName");
if (location == -1) {
    std::cout << "Warning: uniform 'uniformName' not found!" << std::endl;
}
```

**2. 输出中间值作为颜色：**

```glsl
// 调试：把某个值可视化
FragColor = vec4(vec3(someValue), 1.0);
```

**3. 使用固定颜色测试：**

```glsl
// 先用固定颜色确认着色器工作正常
FragColor = vec4(1.0, 0.0, 0.0, 1.0);  // 红色
```

---

## 11. GLSL 内置函数速查

### 11.1 数学函数

| 函数 | 说明 |
|------|------|
| `abs(x)` | 绝对值 |
| `sign(x)` | 符号 (-1, 0, 1) |
| `floor(x)` | 向下取整 |
| `ceil(x)` | 向上取整 |
| `fract(x)` | 小数部分 |
| `mod(x, y)` | 取模 |
| `min(x, y)` | 最小值 |
| `max(x, y)` | 最大值 |
| `clamp(x, min, max)` | 限制范围 |
| `mix(x, y, a)` | 线性插值 |
| `step(edge, x)` | 阶跃函数 |
| `smoothstep(e0, e1, x)` | 平滑阶跃 |

### 11.2 三角函数

| 函数 | 说明 |
|------|------|
| `sin(x)` | 正弦 |
| `cos(x)` | 余弦 |
| `tan(x)` | 正切 |
| `asin(x)` | 反正弦 |
| `acos(x)` | 反余弦 |
| `atan(y, x)` | 反正切 |
| `radians(deg)` | 角度转弧度 |
| `degrees(rad)` | 弧度转角度 |

### 11.3 指数函数

| 函数 | 说明 |
|------|------|
| `pow(x, y)` | x 的 y 次方 |
| `exp(x)` | e 的 x 次方 |
| `log(x)` | 自然对数 |
| `exp2(x)` | 2 的 x 次方 |
| `log2(x)` | 以 2 为底的对数 |
| `sqrt(x)` | 平方根 |
| `inversesqrt(x)` | 平方根的倒数 |

### 11.4 向量函数

| 函数 | 说明 |
|------|------|
| `length(v)` | 向量长度 |
| `distance(p0, p1)` | 两点距离 |
| `dot(v1, v2)` | 点积 |
| `cross(v1, v2)` | 叉积 (vec3) |
| `normalize(v)` | 归一化 |
| `reflect(I, N)` | 反射向量 |
| `refract(I, N, eta)` | 折射向量 |

### 11.5 矩阵函数

| 函数 | 说明 |
|------|------|
| `matrixCompMult(m1, m2)` | 逐分量乘法 |
| `transpose(m)` | 转置 |
| `inverse(m)` | 逆矩阵 |
| `determinant(m)` | 行列式 |

---

## 总结

恭喜完成 Day03 的学习！你已经掌握了：

1. ✅ GLSL 语言的基本语法
2. ✅ 向量和矩阵的数据类型
3. ✅ Swizzling（向量重组）技巧
4. ✅ 顶点着色器和片段着色器之间的数据传递
5. ✅ Uniform 变量的使用
6. ✅ 多顶点属性的配置
7. ✅ Shader 类的封装

**下一步学习建议：**

- 学习 [Textures](https://learnopengl.com/Getting-started/Textures) 章节，给物体贴上纹理
- 学习 [Transformations](https://learnopengl.com/Getting-started/Transformations) 章节，实现旋转、缩放、平移
- 使用 GLM 数学库进行矩阵运算

---

## 参考资料

- [LearnOpenGL - Shaders](https://learnopengl.com/Getting-started/Shaders)
- [LearnOpenGL 中文版 - 着色器](https://learnopengl-cn.github.io/01%20Getting%20started/05%20Shaders/)
- [GLSL 规范](https://www.khronos.org/registry/OpenGL/specs/gl/GLSLangSpec.4.60.pdf)
- [OpenGL Reference Pages](https://www.khronos.org/registry/OpenGL-Refpages/gl4/)

---

*Happy Coding!*

