### Day06 - 坐标系统 (Coordinate Systems)

> **Day06 目标**：理解从 3D 坐标到 2D 屏幕的 **5 个坐标空间**转换过程，学会使用 **MVP 矩阵** (Model-View-Projection) 渲染真正的 3D 物体（例如：旋转的立方体），并开启 **深度测试 (Z-Buffer)** 解决遮挡问题。

---

### 1) 核心知识点：五大坐标空间

OpenGL 的顶点的变换过程，就是把坐标在不同的“空间”里转换，最终变成屏幕上的像素。

**流程图**：
`局部坐标 (Local)` -> [Model Matrix] -> `世界坐标 (World)` -> [View Matrix] -> `观察坐标 (View)` -> [Projection Matrix] -> `裁剪坐标 (Clip)` -> [Viewport Transform] -> `屏幕坐标 (Screen)`

#### 1.1 局部空间 (Local Space)
- **定义**：物体自己的坐标系。
- **例子**：你在建模软件里建一个立方体，它的中心通常在 `(0,0,0)`。无论在这个世界里把它放到哪里，对于它自己来说，它永远在自己的中心。

#### 1.2 世界空间 (World Space)
- **定义**：游戏世界的统一坐标系。
- **变换 (Model Matrix)**：通过 **平移、旋转、缩放**，把物体从“原点”摆放到世界中它该在的地方。
- **公式**：`WorldPos = Model * LocalPos`

#### 1.3 观察空间 (View Space) / 摄像机空间
- **定义**：以**摄像机**为原点的坐标系。
- **变换 (View Matrix)**：把整个世界“移动”到摄像机面前。
- **直觉**：摄像机向后退 = 整个世界向前进。**OpenGL 本身没有摄像机**，我们通过逆向移动世界来模拟摄像机。
- **公式**：`ViewPos = View * WorldPos`

#### 1.4 裁剪空间 (Clip Space)
- **定义**：决定哪些点在屏幕内，哪些在屏幕外。
- **变换 (Projection Matrix)**：
    1.  **正射投影 (Orthographic)**：不近大远小，平行线仍然平行（用于 2D / 工程制图）。
    2.  **透视投影 (Perspective)**：**近大远小**，模拟人眼（用于 3D 游戏）。
- **结果**：坐标范围被变换到 `[-1.0, 1.0]` (NDC)。超出这个范围的顶点会被“裁剪”掉。
- **公式**：`ClipPos = Projection * ViewPos`

#### 1.5 屏幕空间 (Screen Space)
- **定义**：最终像素的 `(x, y)` 坐标。
- **变换**：由 `glViewport` 定义，GPU 自动完成。

---

### 2) 什么是 MVP 矩阵？

这是 3D 渲染中最著名的公式：

$$ V_{clip} = M_{projection} \cdot M_{view} \cdot M_{model} \cdot V_{local} $$

代码实现（GLM）：

```cpp
// 1. Model: 把物体放到世界坐标 (1,1,1) 并旋转
glm::mat4 model = glm::mat4(1.0f);
model = glm::translate(model, glm::vec3(1.0f, 1.0f, 1.0f));
model = glm::rotate(model, glm::radians(45.0f), glm::vec3(1.0f, 0.0f, 0.0f));

// 2. View: 摄像机向后退 3 个单位 (也就是把场景向前移 3 个单位)
glm::mat4 view = glm::mat4(1.0f);
view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f)); 

// 3. Projection: 45度视野, 屏幕宽高比, 近平面0.1, 远平面100
glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)width / height, 0.1f, 100.0f);
```

**传入 Shader**：
通常这三个矩阵会分开传入，或者在 CPU 端乘好传入（但在学习阶段，建议分开传入以保持清晰）。

---

### 3) 深度测试 (Z-Buffer)

当你画一个立方体时，后面的面可能会盖住前面的面（取决于绘制顺序），这是不对的。

- **解决方法**：开启 Z-Buffer。
- **原理**：OpenGL 存储每个像素的深度值（z值）。当要画新像素时，比较它的 z 值和缓存里的 z 值。如果新像素更“近”，就覆盖；否则丢弃。

**代码开启**：

```cpp
// 初始化时开启
glEnable(GL_DEPTH_TEST);

// 渲染循环里清除深度缓冲
glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
```

---

### 4) Day06 易错点 Checklist

1.  **矩阵乘法顺序**：Shader 里必须是 `Projection * View * Model * vec4(pos, 1.0)`。顺序反了会导致物体消失或变形。
2.  **Projection 的 Aspect Ratio**：`glm::perspective` 的第二个参数必须是 `float` 强转的 `width / height`。如果是整数除法（如 `800 / 600`）结果是 `1`，会导致图像被拉伸。
3.  **Z-Buffer 未开启**：如果你看到奇怪的透视（后面的面画到了前面），检查 `glEnable(GL_DEPTH_TEST)` 和 `glClear(... | GL_DEPTH_BUFFER_BIT)`。
4.  **纹理翻转**：`stbi_set_flip_vertically_on_load(true)` 在 3D 物体贴图时依然重要，否则贴图是上下颠倒的。

---

### 5) 练习

- **练习 1**：修改 `projection` 的 FOV（视野角度），观察近大远小的变化。
- **练习 2**：修改 `view` 矩阵，让“摄像机”左右移动。
- **练习 3**：画 10 个立方体，每个位置不同，旋转角度不同（使用循环，每次 draw 前更新 Model 矩阵）。
