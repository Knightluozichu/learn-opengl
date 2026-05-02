### Day05 - 变换（Transformations）+ GLM（完整知识点）

> Day05 的目标：在 CPU 侧用 **GLM** 生成变换矩阵（平移/旋转/缩放），通过 **uniform mat4** 传入顶点着色器，让物体产生“移动/旋转/缩放”等效果，并理解**矩阵顺序**为什么会导致完全不同的视觉结果。

<div style="border:2px solid #2ecc71; background:#eafaf1; padding:12px; border-radius:8px;">
<b>提醒（绿色）</b><br/>
GLM 是 <b>header-only</b> 库：只要把 <code>glm/</code> 放进工程的 <code>include/</code>，然后 <code>#include &lt;glm/glm.hpp&gt;</code> 就能用，不需要放到 <code>lib/</code>，也不需要链接。更多信息见官方页：<a href="https://glm.g-truc.net/0.9.8/index.html">GLM 0.9.8</a>。
</div>

---

### 1) 你今天要掌握什么（Checklist）

- **GLM 基础类型**：`glm::vec2/3/4`、`glm::mat4`
- **变换矩阵的构建**：`glm::translate / glm::rotate / glm::scale`
- **把 mat4 传给 shader**：`glm::value_ptr(mat)` + `glUniformMatrix4fv`
- **矩阵乘法顺序**：为什么 `R*T` 和 `T*R` 视觉完全不一样
- **绕任意点旋转（pivot）**：`T(p) * R * T(-p)`
- **调试思路**：shader 路径/资源路径/纹理单元/uniform location

---

### 2) 本项目里的运行时路径约定（非常重要）

- **运行时从 `build/` 目录启动**（例如 `cd build && ./day05`）
- **shader**：构建时会同步到 `build/shaders/day05/`
- **图片资源**：构建时会同步到 `build/assets/day05/`

<div style="border:2px solid #f1c40f; background:#fff8db; padding:12px; border-radius:8px;">
<b>警告（黄色）</b><br/>
Day05 一定要加载 <b>day05 的 shader</b>（例如 <code>shaders/day05/vertex.glsl</code>），因为 Day05 vertex shader 里才有 <code>uniform mat4 transform</code>。<br/>
如果你误加载了 day04 的 shader，<code>setMat4("transform", ...)</code> 会被静默忽略（location = -1），视觉上就“没效果”。
</div>

---

### 3) GLM 常用 include（你 day05 会用到）

```cpp
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp> // translate/rotate/scale
#include <glm/gtc/type_ptr.hpp>         // value_ptr
```

---

### 4) mat4 必须初始化为单位矩阵（非常关键）

```cpp
glm::mat4 trans(1.0f); // ✅ 单位矩阵
```

<div style="border:2px solid #e74c3c; background:#ffe9e7; padding:12px; border-radius:8px;">
<b>易错（红色）</b><br/>
<code>glm::mat4 trans;</code> 在很多情况下并不是你期望的“单位矩阵”，会导致：画面抖动、物体飞走、甚至直接看不见。<br/>
务必写：<code>glm::mat4 trans(1.0f);</code>
</div>

---

### 5) 变换矩阵怎么传进 shader（uniform mat4）

顶点着色器（典型写法）：

```glsl
uniform mat4 transform;
gl_Position = transform * vec4(aPos, 1.0);
```

CPU 侧（典型写法）：

```cpp
ourShader.use();
ourShader.setMat4("transform", glm::value_ptr(trans));
```

<div style="border:2px solid #f1c40f; background:#fff8db; padding:12px; border-radius:8px;">
<b>警告（黄色）</b><br/>
如果 uniform 名字拼错 / shader 没用到 / 你用错 shader 文件：<code>glGetUniformLocation</code> 会返回 -1，OpenGL 会“悄悄不设置”，你就会觉得“没效果”。<br/>
建议调试时打印：uniform location 是否为 -1。
</div>

---

### 6) 矩阵乘法的公式（你问过的重点）

两个 4×4 矩阵 \(C=A\cdot B\) 的元素计算：

\[
C_{ij} = \sum_{k=0}^{3} A_{ik}\,B_{kj}
\]

矩阵乘向量（以列向量为例）：

\[
v' = M \cdot v
\]

> **关键**：矩阵乘法满足结合律，但**不满足交换律**，所以顺序不同效果就不同。

---

### 7) 为什么 rotate / translate 顺序会导致视觉完全不同

在顶点着色器里是：

\[
gl\_Position = M \cdot v
\]

当 \(M\) 是多个变换相乘时：

\[
M = M_1 \cdot M_2 \cdot \dots \cdot M_n
\]

因为 \(v\) 在最右边，所以 **最右边的矩阵最先作用到顶点**。

#### 直觉判断：在旋转发生时物体是否已经“离开原点”

- **旋转发生前已经平移到某个偏移位置** → 很容易看到“绕原点公转”
- **旋转发生在局部原点（物体中心）** → 更像“自转”

<div style="border:2px solid #2ecc71; background:#eafaf1; padding:12px; border-radius:8px;">
<b>提醒（绿色）</b><br/>
想验证“到底绕哪里转”，不要只靠眼睛：打印变换后某个点的位置（比如中心点），或同时画两个物体对比两种顺序，会更直观。
</div>

---

### 8) 绕任意点（pivot）旋转：标准公式与代码

如果你想绕点 \(p\) 旋转（而不是绕 (0,0)），使用：

\[
M = T(p)\cdot R \cdot T(-p)
\]

代码示例：

```cpp
glm::vec3 p(0.5f, -0.5f, 0.0f);
glm::mat4 m(1.0f);
m = glm::translate(m, p);
m = glm::rotate(m, angle, glm::vec3(0,0,1));
m = glm::translate(m, -p);
```

---

### 9) 纹理（延续 Day04，但 Day05 常一起用）

- **两张纹理**：
  - `texture1.bind(0);` + `setInt("ourTexture", 0);`
  - `texture2.bind(1);` + `setInt("ourTexture2", 1);`
- **片段着色器混合**：`mix(tex1, tex2, factor)`

<div style="border:2px solid #e74c3c; background:#ffe9e7; padding:12px; border-radius:8px;">
<b>易错（红色）</b><br/>
shader 里用了两张纹理（unit 0 和 1），但 draw 前只绑定了其中一张 → macOS 常见日志：unit 1 unloadable / using zero texture，画面黑/异常。<br/>
记住：用几张纹理，就每帧都绑定几张到对应单元。
</div>

---

### 10) Day05 “没效果”时的最快排错顺序（按概率从高到低）

1. **是否加载了 day05 的 shader 路径？**（不是 day04）
2. **shader 是否编译/链接成功？**（看控制台 log）
3. **uniform location 是否为 -1？**（transform / sampler）
4. **`glm::mat4` 是否初始化为 `1.0f`？**
5. **纹理路径是否对（从 build 目录运行时）？**（assets/day05/...）
6. **纹理单元是否都绑定？**（0/1）
7. **顶点属性 layout(location=0/1/2) 是否匹配 VAO 配置？**

---

### 11) 建议练习（做完 Day05 更扎实）

- **练习 A**：分别实现“自转”和“公转”，并用打印中心点坐标验证轨迹差异
- **练习 B**：实现绕任意 pivot 点旋转（用 `T(p)*R*T(-p)`）
- **练习 C**：用键盘控制 `mix` 比例（uniform float）
