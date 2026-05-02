# Day04 - 纹理（Textures）知识体系（完整）

> 对应教程：[LearnOpenGL CN - 纹理](https://learnopengl-cn.github.io/01%20Getting%20started/06%20Textures/)
>
> Day04 的目标：把“贴图”真正跑起来：从图片文件 → 生成 OpenGL 纹理对象 → 在片段着色器里采样 → 绘制到屏幕。

---

## 目录

1. [你今天要掌握什么](#你今天要掌握什么)
2. [纹理是什么（为什么需要）](#纹理是什么为什么需要)
3. [纹理坐标与采样](#纹理坐标与采样)
4. [纹理对象的生命周期（创建→配置→上传→使用→销毁）](#纹理对象的生命周期创建配置上传使用销毁)
5. [关键概念一：纹理环绕（Wrap）](#关键概念一纹理环绕wrap)
6. [关键概念二：纹理过滤（Filter）](#关键概念二纹理过滤filter)
7. [关键概念三：Mipmap](#关键概念三mipmap)
8. [关键概念四：纹理单元（Texture Unit）与多个纹理](#关键概念四纹理单元texture-unit与多个纹理)
9. [在本项目里怎么落地（目录约定 + shader 同步）](#在本项目里怎么落地目录约定--shader-同步)
10. [Day04 实战步骤（从“有颜色的矩形”到“有贴图的矩形”）](#day04-实战步骤从有颜色的矩形到有贴图的矩形)
11. [常见坑位与排错清单](#常见坑位与排错清单)
12. [练习（建议做完再进 Day05）](#练习建议做完再进-day05)

---

## 你今天要掌握什么

- **纹理坐标 (Texture Coordinates / UV)**：每个顶点不仅有位置，还要告诉 GPU“贴图的哪一块贴到这个顶点上”。
- **采样 (Sampling)**：在片段着色器里用 `texture(sampler2D, TexCoord)` 从纹理取颜色。
- **纹理对象 (OpenGL Texture Object)**：`glGenTextures → glBindTexture → glTexImage2D → glGenerateMipmap`。
- **环绕/过滤/Mipmap**：决定纹理坐标超范围怎么处理、缩放时怎么取样、更小尺寸的预生成纹理链。
- **纹理单元**：一个着色器里可以用多张纹理（`GL_TEXTURE0/1/...`），并用 `uniform sampler2D` 选择使用哪张。

---

## 纹理是什么（为什么需要）

纹理（Texture）本质是一张 2D 图片（也可以是 1D/3D），用来给物体增加细节。

如果只靠“顶点颜色”，想要更精细的图案就需要更多顶点，开销很大；纹理可以用很少顶点承载大量细节，这也是教程里强调纹理的原因（见：[LearnOpenGL CN - 纹理](https://learnopengl-cn.github.io/01%20Getting%20started/06%20Textures/)）。

---

## 纹理坐标与采样

### 纹理坐标（UV）

- UV 范围通常是 **0 到 1**
- `vec2(0,0)` 对应纹理左下角，`vec2(1,1)` 对应右上角（OpenGL 的约定）

你需要在顶点数据里加入 `TexCoord`（2 个 float），并在顶点着色器把它传给片段着色器：

- 顶点属性：`aTexCoord`
- 传递变量：`out vec2 TexCoord;`

片段着色器里用：

- `uniform sampler2D texture1;`
- `texture(texture1, TexCoord)` 采样

---

## 纹理对象的生命周期（创建→配置→上传→使用→销毁）

### 1) 创建并绑定纹理对象

```cpp
unsigned int texture;
glGenTextures(1, &texture);
glBindTexture(GL_TEXTURE_2D, texture);
```

### 2) 配置纹理参数（Wrap / Filter）

```cpp
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
```

### 3) 上传像素数据（glTexImage2D）

核心就是把图片像素传给 GPU：

```cpp
glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
glGenerateMipmap(GL_TEXTURE_2D);
```

如果图片带 alpha（png 常见），外部格式要用 `GL_RGBA`（教程也强调了这一点：[LearnOpenGL CN - 纹理单元](https://learnopengl-cn.github.io/01%20Getting%20started/06%20Textures/)）。

### 4) 使用纹理（渲染时绑定）

```cpp
glActiveTexture(GL_TEXTURE0);
glBindTexture(GL_TEXTURE_2D, texture);
```

### 5) 销毁（可选）

```cpp
glDeleteTextures(1, &texture);
```

---

## 关键概念一：纹理环绕（Wrap）

当纹理坐标超出 `[0,1]`（比如 2.0），应该怎么处理？

常用模式：

- `GL_REPEAT`：重复
- `GL_MIRRORED_REPEAT`：镜像重复
- `GL_CLAMP_TO_EDGE`：夹到边缘
- `GL_CLAMP_TO_BORDER`：夹到边界色（还要设置 border color）

对应设置的是：

- `GL_TEXTURE_WRAP_S`（U / x 方向）
- `GL_TEXTURE_WRAP_T`（V / y 方向）

---

## 关键概念二：纹理过滤（Filter）

当纹理被放大/缩小时，像素怎么取？

常用模式：

- `GL_NEAREST`：最近点（像素感强）
- `GL_LINEAR`：双线性插值（更平滑）

对应设置：

- `GL_TEXTURE_MIN_FILTER`（缩小）
- `GL_TEXTURE_MAG_FILTER`（放大）

注意：`MIN_FILTER` 可以用带 mipmap 的过滤（比如 `GL_LINEAR_MIPMAP_LINEAR`），`MAG_FILTER` 不行。

---

## 关键概念三：Mipmap

Mipmap 是预先生成的一组“越来越小”的纹理，用于纹理缩小时避免锯齿、提升性能。

生成：

```cpp
glGenerateMipmap(GL_TEXTURE_2D);
```

最常用的 min filter：

- `GL_LINEAR_MIPMAP_LINEAR`（三线性过滤，质量更好）

---

## 关键概念四：纹理单元（Texture Unit）与多个纹理

为什么 `sampler2D` 是 uniform？因为你要告诉着色器“用哪个纹理单元”。

常见流程（两张纹理）：

```cpp
glActiveTexture(GL_TEXTURE0);
glBindTexture(GL_TEXTURE_2D, texture1);
glActiveTexture(GL_TEXTURE1);
glBindTexture(GL_TEXTURE_2D, texture2);

ourShader.use();
glUniform1i(glGetUniformLocation(ourShader.ID, "texture1"), 0);
ourShader.setInt("texture2", 1);
```

片段着色器里：

```glsl
uniform sampler2D texture1;
uniform sampler2D texture2;
FragColor = mix(texture(texture1, TexCoord), texture(texture2, TexCoord), 0.2);
```

（示例与解释来自教程：[LearnOpenGL CN - 纹理单元](https://learnopengl-cn.github.io/01%20Getting%20started/06%20Textures/)）

---

## 在本项目里怎么落地（目录约定 + shader 同步）

你当前工程做了两层工程化约定：

### 1) 运行时 shader 读取路径约定

以 day04 为例，你在代码里写：

```cpp
Shader ourShader("shaders/day04/vertex.glsl", "shaders/day04/fragment.glsl");
```

运行时从 `build/` 目录启动 `./day04`，会读取：

- `build/shaders/day04/vertex.glsl`
- `build/shaders/day04/fragment.glsl`

### 2) shader 自动同步（hash 驱动）

你每次 `make` 或 `make day04` 时，构建系统会对 `src/day04/shaders/**` 做内容 hash：

- **内容变了** → 自动同步到 `build/shaders/day04/`
- **内容没变** → 输出 `unchanged`，不拷贝

所以请只编辑 `src/day04/shaders/*.glsl`，不要直接改 `build/` 下的文件。

---

## Day04 实战步骤（从“有颜色的矩形”到“有贴图的矩形”）

你现在的 `day04/main.cpp` 顶点数据是「位置 + 颜色」，要贴图需要：

### 1) 顶点结构升级：加纹理坐标

把每个顶点从 6 个 float 变成 8 个 float：

- `pos(x,y,z)` 3
- `color(r,g,b)` 3
- `tex(u,v)` 2

矩形 4 个顶点建议这样给 UV（最常见布局）：

- 左下：`(0,0)`
- 右下：`(1,0)`
- 左上：`(0,1)`
- 右上：`(1,1)`

### 2) 顶点属性配置：新增 location=2 的 aTexCoord

stride 变成：`8 * sizeof(float)`  
offset：

- pos：0
- color：`3*sizeof(float)`
- tex：`6*sizeof(float)`

### 3) shader 改造：传 TexCoord + 采样

- 顶点着色器 `out vec2 TexCoord;`
- 片段着色器 `uniform sampler2D texture1;` 并 `texture(texture1, TexCoord)`

### 4) 加载图片：stb_image

教程使用 `stb_image.h`（见：[LearnOpenGL CN - stb_image.h](https://learnopengl-cn.github.io/01%20Getting%20started/06%20Textures/)）。

你可以把图片放在例如：

- `src/day04/assets/container.jpg`

然后工程化同步一份到 `build/assets/day04/`（后面你想做我也可以帮你加：和 shader 同理）。

### 5) 生成纹理对象并绑定到纹理单元

按“生命周期”那套流程把纹理跑起来，最后在 draw 前：

- `glActiveTexture(GL_TEXTURE0); glBindTexture(GL_TEXTURE_2D, tex);`
- `ourShader.use(); ourShader.setInt("texture1", 0);`

---

## 常见坑位与排错清单

1. **图片上下颠倒**：教程建议 `stbi_set_flip_vertically_on_load(true);`（见：[LearnOpenGL CN - 纹理单元](https://learnopengl-cn.github.io/01%20Getting%20started/06%20Textures/)）。
2. **RGB / RGBA 搞错**：png 常带 alpha，要用 `GL_RGBA`。
3. **忘了绑定纹理单元**：先 `glActiveTexture(GL_TEXTURE0)` 再 `glBindTexture`。
4. **sampler2D 没设置到正确纹理单元**：`glUniform1i(location, unitIndex)`。
5. **纹理坐标没传到片段着色器**：顶点 `out` 和片段 `in` 名字必须一致。
6. **改 shader 效果不变**：确认你改的是 `src/day04/shaders/`，然后 `make day04`（hash 会更新并同步到 build）。

---

## 练习（建议做完再进 Day05）

来自教程练习（见：[LearnOpenGL CN - 练习](https://learnopengl-cn.github.io/01%20Getting%20started/06%20Textures/)）：

1. **只让笑脸反向**：在片段着色器翻转第二张纹理的 `TexCoord.x`。
2. **把 UV 扩展到 0~2**：测试不同 wrap 模式，让一个矩形角上出现 4 个小脸。
3. **只显示纹理中间一部分**：改 UV 只采样中间区域，配合 `GL_NEAREST` 看清像素格。
4. **用 uniform 控制 mix 比例**：上下键改变两张纹理混合比例（CPU 传 uniform）。

---

## 参考链接

- [LearnOpenGL CN - 纹理](https://learnopengl-cn.github.io/01%20Getting%20started/06%20Textures/)


---

## 本次练习踩坑总结（易错提示）

下面这些都是这次 day04 练习里**真实出现过**、最容易让画面“全黑/不对劲”的点，建议你以后按这个顺序排查。

### 1) 纹理单元 & sampler uniform 没对应上（最常见）

- **现象**：窗口正常，但矩形是黑的；或 macOS 控制台出现类似 “unit 1 ... unloadable ... using zero texture”。
- **根因**：shader 里 `sampler2D` 指向了某个纹理单元（例如 1），但 CPU 侧没把对应纹理绑定到该单元。
- **要点**：
  - 每张纹理在 draw 前都要绑定到正确单元：`texture1.bind(0); texture2.bind(1);`
  - sampler 只接收“纹理单元索引”，不是纹理 ID：`setInt("ourTexture", 0); setInt("ourTexture2", 1);`

### 2) 资源路径/运行目录不一致，导致 `stbi_load` 实际加载失败

- **现象**：你以为加载成功了，但实际 `stbi_load` 失败；最终采样全黑。
- **根因**：从 `build/` 运行 `./day04` 时，相对路径是以 `build/` 为基准；图片不在 `build/assets/day04/` 就会找不到。
- **要点**：
  - 建议约定：运行时从 `build/` 启动，资源路径写 `assets/day04/xxx`
  - 构建系统需要把 `src/day04/*.jpg/*.png` 同步到 `build/assets/day04/`
  - 加载失败时要把原因打印出来：`stbi_failure_reason()`

### 3) `stb_image` 的实现宏放错位置（会引发 include/重复定义问题）

- **错误写法**：在头文件里 `#define STB_IMAGE_IMPLEMENTATION` + `#include "stb_image.h"`
- **后果**：头文件被多处 include 时，容易产生重复符号/链接冲突；IDE 也更容易出现混乱诊断。
- **正确写法**：
  - 在**一个** `.cpp` 里写：`#define STB_IMAGE_IMPLEMENTATION` 然后 `#include <stb_image.h>`
  - 其他需要用 stb API 的地方只 `#include <stb_image.h>`

### 4) include 顺序错误：`glad` 必须在 `GLFW` 之前

- **现象**：编译报错 “OpenGL header already included, glad already provides it”
- **要点**：固定写法：
  - `#include <glad/glad.h>`
  - `#include <GLFW/glfw3.h>`

### 5) 两张纹理混合时，第二张纹理忘记绑定（导致 unit 1 用零纹理）

- **现象**：fragment shader `mix(texture1, texture2, ...)` 后画面变黑/异常；控制台提示 unit 1 unloadable。
- **要点**：shader 用了几张纹理，draw 前就必须绑定几张；不绑定就等于“采样空气”。

### 6) PNG 通道数/格式没处理好（RGB vs RGBA）

- **现象**：颜色不对、alpha 不对、甚至看起来发黑。
- **要点**：用 `channels` 选择 `GL_RED/GL_RGB/GL_RGBA`，并建议开启混合：
  - `glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);`

### 7) 图片原点与纹理坐标原点不一致（通常表现为上下颠倒）

- **现象**：贴图“倒着”了（但通常不会全黑）。
- **要点**：`stbi_set_flip_vertically_on_load(true);`

### 8) IDE 提示“找不到 texutre.cpp 在 compile_commands.json 里”

- **含义**：CMake 的编译数据库没收录这个文件，IDE 只能退回用 `includePath` 猜测，容易误报头文件找不到。
- **解决**：新增/改名 `.cpp` 后跑一次：
  - `cmake -S . -B build`

