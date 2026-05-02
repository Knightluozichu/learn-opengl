### Day08 - 颜色（Colors）：进入光照篇的第一课

> **Day08 目标**：完成 Day07 的摄像机系统后正式进入 **光照（Lighting）** 篇。本课先**不做真正的光照计算**，而是搭建好后续所有光照课程都会用到的基础场景：**一个被照射的立方体 + 一个代表光源的立方体**，并通过 `lightColor * objectColor` 这种「逐分量相乘」的方式，建立对“物体颜色 = 光源颜色 × 物体反射颜色”的物理直觉。
>
> 对应教程：[LearnOpenGL CN - 颜色](https://learnopengl-cn.github.io/02%20Lighting/01%20Colors/)

#### Ready
- `src/day08/main.cpp` — 颜色相乘 + 共享 VBO 双 VAO + 两套 shader 场景
- `src/day08/shaders/lighting.vs` / `lighting.fs` — 被照射物体 shader（`lightColor * objectColor`）
- `src/day08/shaders/light_cube.vs` / `light_cube.fs` — 光源立方体 shader（纯白）
- `src/day08/Common/` — 复用 Day07 的 Camera / Shader / Texture

---

### 1) 你今天要掌握什么（Checklist）

- **颜色的物理含义**：`观察到的颜色 = 光源颜色 ⊙ 物体反射颜色`（⊙ 表示逐分量相乘）
- **场景结构**：同一个立方体几何 + **两个 shader 程序** + **两个 VAO**
  - 「**被照射物体**」shader：接收 `objectColor` 和 `lightColor`
  - 「**光源**」shader：直接输出白色（光源永远是亮的）
- **共享 VBO 的多 VAO 写法**：节省显存、减少重复上传
- **延续 Day07 的相机**：`view / projection` 仍然由 `Camera` 提供
- **为 Day09 (Phong 基础光照) 做准备**：本课**还没有法线、还没有点积**，下一课才开始算漫反射

---

### 2) 本项目的运行时路径约定（沿用 Day04~07）

- **从 `build/` 启动**：`cd build && ./day08`
- **shader**：构建时同步到 `build/shaders/day08/`
- **资源**：构建时同步到 `build/assets/day08/`（本课暂时不需要纹理，留好目录即可）

<div style="border:2px solid #f1c40f; background:#fff8db; padding:12px; border-radius:8px;">
<b>警告（黄色）</b><br/>
Day08 一定要加载 <b>day08 的 shader</b>。本课新增了 <code>uniform vec3 objectColor</code> 和 <code>uniform vec3 lightColor</code>，如果误用 day07 的 shader，<code>setVec3(...)</code> 会被静默忽略（location = -1），你看到的就是“纯色立方体没反应”。
</div>

---

### 3) 颜色的物理理解（最关键的一段）

现实世界里，物体本身**不发光**，我们看到的是**物体反射进入眼睛的光**。

设：

- 光源颜色：\(L = (L_r, L_g, L_b)\)
- 物体反射颜色（也就是我们平时说的“物体颜色”）：\(C = (C_r, C_g, C_b)\)

最终我们看到的颜色为：

\[
F = L \odot C = (L_r \cdot C_r,\; L_g \cdot C_g,\; L_b \cdot C_b)
\]

举例（教程里的经典例子）：

- 白光 `(1, 1, 1)` 照在珊瑚色物体 `(1.0, 0.5, 0.31)` → 反射出 `(1.0, 0.5, 0.31)`
- 绿光 `(0, 1, 0)` 照在同一个物体 → 反射出 `(0.0, 0.5, 0.0)`（暗绿色）
- 红光 `(1, 0, 0)` 照在同一个物体 → 反射出 `(1.0, 0.0, 0.0)`（纯红）

> **直觉**：物体只能反射它颜色里**已经有的分量**。绿光照红色物体看起来是黑的，因为红色物体在绿色通道几乎不反射。

---

### 4) 场景结构：两个立方体 + 两套 shader

Day08 的画面里有两个立方体，但它们**几何完全相同**（一个 36 顶点的立方体），区别在于：

| 角色 | model 矩阵 | 使用的 shader | 输出颜色 |
|------|-----------|--------------|---------|
| 被照射物体（cube） | 放在世界中央，正常大小 | `lighting.vs / lighting.fs` | `objectColor * lightColor` |
| 光源（light cube）| 移到光源位置，缩小 0.2 倍 | `light_cube.vs / light_cube.fs` | 直接输出白色 |

#### 物体 shader（关键片段）

```glsl
// fragment.glsl —— 被照射物体
#version 330 core
out vec4 FragColor;

uniform vec3 objectColor;
uniform vec3 lightColor;

void main() {
    FragColor = vec4(lightColor * objectColor, 1.0);
}
```

#### 光源 shader（关键片段）

```glsl
// light_cube.fs —— 光源自身
#version 330 core
out vec4 FragColor;

void main() {
    FragColor = vec4(1.0); // 永远是白
}
```

---

### 5) 共享 VBO 的「多 VAO」写法

立方体几何只有一份，没必要传两次。常见写法：

```cpp
unsigned int VBO, cubeVAO, lightVAO;
glGenBuffers(1, &VBO);
glGenVertexArrays(1, &cubeVAO);
glGenVertexArrays(1, &lightVAO);

// 1) 上传一次顶点数据
glBindBuffer(GL_ARRAY_BUFFER, VBO);
glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

// 2) cubeVAO：被照射物体
glBindVertexArray(cubeVAO);
glBindBuffer(GL_ARRAY_BUFFER, VBO);
glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
glEnableVertexAttribArray(0);

// 3) lightVAO：光源（共享同一个 VBO，只需要 position）
glBindVertexArray(lightVAO);
glBindBuffer(GL_ARRAY_BUFFER, VBO);
glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
glEnableVertexAttribArray(0);
```

<div style="border:2px solid #2ecc71; background:#eafaf1; padding:12px; border-radius:8px;">
<b>提醒（绿色）</b><br/>
VAO 记录的是「<b>顶点属性指针的状态</b>」，包括当前绑定的 VBO。所以即使两个 VAO 共用同一个 VBO，只要分别在两个 VAO 上 <code>glVertexAttribPointer</code> 一遍，状态就分别保存下来了，draw 的时候 <code>glBindVertexArray(...)</code> 即可切换。
</div>

---

### 6) 渲染循环骨架

```cpp
glm::vec3 lightPos(1.2f, 1.0f, 2.0f);
glm::vec3 objectColor(1.0f, 0.5f, 0.31f);
glm::vec3 lightColor (1.0f, 1.0f, 1.0f);

while (!glfwWindowShouldClose(window)) {
    // ... deltaTime / processInput / clear ...

    glm::mat4 view       = camera.getViewMatrix();
    glm::mat4 projection = glm::perspective(glm::radians(camera.getZoom()),
                                            (float)W/H, 0.1f, 100.0f);

    // ---- 1) 画被照射的立方体 ----
    lightingShader.use();
    lightingShader.setVec3("objectColor", objectColor);
    lightingShader.setVec3("lightColor",  lightColor);
    lightingShader.setMat4("view",        view);
    lightingShader.setMat4("projection",  projection);

    glm::mat4 model(1.0f);
    lightingShader.setMat4("model", model);
    glBindVertexArray(cubeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    // ---- 2) 画光源立方体 ----
    lightCubeShader.use();
    lightCubeShader.setMat4("view",       view);
    lightCubeShader.setMat4("projection", projection);

    model = glm::mat4(1.0f);
    model = glm::translate(model, lightPos);
    model = glm::scale(model, glm::vec3(0.2f)); // 让光源看起来小一点
    lightCubeShader.setMat4("model", model);
    glBindVertexArray(lightVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    glfwSwapBuffers(window);
    glfwPollEvents();
}
```

---

### 7) Day08 易错点 Checklist

1. **加载错 shader**：还在用 day07 的 shader → `objectColor / lightColor` 永远 location = -1，画面无变化。
2. **两个 VAO 漏配置一个**：忘了在 `lightVAO` 上 `glVertexAttribPointer` → 画光源时拿到的是上一次的属性配置，可能整块乱。
3. **画完物体没切 shader**：直接 `glDrawArrays` 第二个立方体，结果两个都用同一个 shader。永远先 `lightCubeShader.use();` 再 draw。
4. **忘开深度测试**：沿用 Day07 的设置，记得 `glEnable(GL_DEPTH_TEST);` 和每帧 `glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);`。
5. **光源位置和 model 矩阵没对上**：`lightPos` 改了之后，画光源那块的 `glm::translate(model, lightPos)` 也要同步更新（建议把 `lightPos` 抽成变量，两处共用）。
6. **过早纠结“为啥没明暗”**：本课**不计算光照**，所以被照射的立方体看起来是**纯色平面**，没有立体感是正常的——立体感要等 Day09 的漫反射。

<div style="border:2px solid #e74c3c; background:#ffe9e7; padding:12px; border-radius:8px;">
<b>易错（红色）</b><br/>
不要在 Day08 就尝试 <code>dot(normal, lightDir)</code>，因为本课的顶点数据<b>还没有法线</b>。Day09 会专门把顶点结构升级成 <code>位置 + 法线</code>，那时再做漫反射才不会乱。
</div>

---

### 8) 与前几天的串联

- **Day04 / Day05**：你已经会用 uniform 把 CPU 数据（纹理单元、`mat4`）传进 shader → 现在传的是 `vec3` 颜色，本质完全一样。
- **Day06 / Day07**：MVP 与摄像机继续生效，本课**完全不需要改 view/projection 的获取方式**，只是给一个新 shader 多设两个 uniform 而已。
- **Day09 预告**：在本课的 `lightingShader` 里加上**法线属性 + 法线变换 + 漫反射点积**，就能让立方体真正“被照亮”——也就是 Phong 模型的第一步。

---

### 9) 练习

- **练习 1**：把 `lightColor` 改成随时间变化的彩色（例如用 `sin(time)` 分别驱动 RGB），观察被照射物体颜色的变化，**亲眼验证「逐分量相乘」**。
- **练习 2**：让光源立方体绕物体公转（用 Day05 学过的 `T(p) * R * T(-p)` 思想），同时同步更新 `lightingShader` 里要用到的 `lightPos`（为 Day09 做铺垫，本课暂时还用不上 `lightPos`）。
- **练习 3**：把光源颜色设为 `(1, 0, 0)`、物体颜色设为 `(0, 1, 0)`，预测画面颜色再运行验证（应该接近黑色）。
- **练习 4**：尝试用 Day07 的 WASD + 鼠标飞到光源旁边、再绕到物体背面看看——会发现**两个面看起来一样亮**，这正是“没有真正光照”的直观证据，给 Day09 一个非常好的对比基线。

---

### 参考链接

- [LearnOpenGL CN - 颜色](https://learnopengl-cn.github.io/02%20Lighting/01%20Colors/)
- 下一课预习：[LearnOpenGL CN - 基础光照](https://learnopengl-cn.github.io/02%20Lighting/02%20Basic%20Lighting/)
