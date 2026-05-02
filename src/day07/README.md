### Day07 - 摄像机系统 (Camera Systems)

> **Day07 目标**：在 Day06 的坐标系统基础上，封装一个可复用的 **摄像机类 `Camera`**，通过 **WASD 键盘 + 鼠标移动** 实现第一人称视角在 3D 场景中自由走动，并用摄像机的 `view / projection` 矩阵驱动渲染。

---

### 1) 从“动场景”到“动摄像机”

在 Day06 里，`view` 矩阵是手动写死的，例如：

```cpp
glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -3.0f));
```

本质上是“**把整个世界往后推 3 个单位**”，相当于“假装有一台摄像机在原点往前看”。

Day07 做了两件升级的事：

- **封装摄像机状态**：位置 `position`、朝向 `front`、上方向 `up`、右方向 `right`、偏航角 `yaw`、俯仰角 `pitch` 等，都放进 `Camera` 类。
- **实时更新 view / projection**：
  - `view = camera.getViewMatrix();`
  - `projection = glm::perspective(glm::radians(camera.getZoom()), aspect, 0.1f, 100.0f);`

这样，按键或鼠标一动，摄像机的状态就变了，`view / projection` 也会随之改变。

---

### 2) `Camera` 类的核心接口

主要看 `Common/camera.h` 和 `Common/camera.cpp`：

- **构造函数**：
  - 接收 `position`、`up`、`yaw`、`pitch`，并在初始化列表里设置默认的 `front`、移动速度 `movementSpeed`、鼠标灵敏度 `mouseSensitivity`、缩放 `zoom`。
- **对外接口**：
  - `glm::mat4 getViewMatrix() const`：返回当前摄像机的 `view` 矩阵，本质是 `glm::lookAt(position, position + front, up)`。
  - `float getZoom() const`：返回当前“视野角度”FOV，用来构造 `projection` 矩阵。
  - `void processKeyboard(CameraMovement direction, float deltaTime)`：
    - `W / S`：沿着 `front` 前进 / 后退。
    - `A / D`：沿着 `right` 左移 / 右移。
    - 使用 `movementSpeed * deltaTime` 保证不同帧率下移动速度一致。
  - `void processMouseMovement(float xoffset, float yoffset, bool constrainPitch)`：
    - 鼠标在 x 方向移动 -> 改变 `yaw`（左右转头）。
    - 鼠标在 y 方向移动 -> 改变 `pitch`（上下抬头/低头）。
    - 可选地限制 `pitch` 在 \[-89°, 89°]，避免“抬头抬到翻过去”。
  - `void processMouseScroll(float yoffset)`：
    - 用滚轮调整 `zoom`（FOV），并夹在 \[1°, 45°] 区间内（本示例暂未在 `main.cpp` 里挂上滚轮回调，但接口已经准备好）。

内部有一个重要的私有函数：

- `updateCameraVectors()`：
  - 根据当前 `yaw` / `pitch` 计算新的 `front` 向量，再由此得到 `right = normalize(cross(front, worldUp))` 和 `up`。
  - 这一步就是把“欧拉角 (yaw, pitch)” 转换成真正用于渲染的方向向量。

---

### 3) 输入系统：WASD + 鼠标 + TAB

在 `main.cpp` 中：

- **键盘输入**：`processInput(GLFWwindow *window, Camera &camera, float deltaTime)`
  - `ESC`：退出。
  - `W / A / S / D`：调用 `camera.processKeyboard(...)`。
  - `TAB`：在“鼠标锁定到窗口中心”与“自由鼠标”之间切换：
    - 锁定：`glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);`
    - 解锁：`glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);`
- **鼠标移动**：`mouse_callback(GLFWwindow *window, double xpos, double ypos)`
  - 通过 `glfwSetWindowUserPointer(window, &camera);` 把 `Camera*` 绑到窗口上，在回调里取出来：
    - `Camera *camera = (Camera *)glfwGetWindowUserPointer(window);`
  - 计算 `xoffset / yoffset`，再调用 `camera->processMouseMovement(xoffset, yoffset);`
  - 只有当 `cursorDisabled == true` 时才处理鼠标（避免解锁鼠标时画面乱转）。

效果：按住 `WASD` 可以在由 100 个随机分布的立方体构成的 3D 世界中穿行，移动鼠标就像在 FPS 游戏里“转头看四周”。

---

### 4) 渲染部分回顾

与 Day06 相比，渲染管线本身变化不大：

- **模型数据**：一个带纹理坐标的立方体，通过 `indices` 画出 6 个面。
- **场景布置**：用随机数生成 100 个 `cubePositions`，循环中：
  - 先 `translate` 到各自的位置。
  - 再按时间做一点旋转，让每个立方体动起来。
- **MVP**：
  - `model`：每个立方体自己的平移 + 旋转。
  - `view`：来自 `camera.getViewMatrix()`。
  - `projection`：用 `camera.getZoom()` 作为 FOV 构造透视投影。

核心思想：**不再手写一个“固定的” view，而是让 Camera 决定“我现在在世界的哪个位置、朝向哪里看”。**

---

### 5) Day07 易错点 Checklist

1. **忘记设置 WindowUserPointer**：
   - 必须在创建完 `Camera` 之后调用 `glfwSetWindowUserPointer(window, &camera);`，否则鼠标回调拿不到摄像机。
2. **鼠标 Y 轴方向搞反**：
   - 一般会用 `yoffset = lastY - (float)ypos;` 来让“鼠标往上移动 = 抬头”。
3. **`deltaTime` 忽略导致不同帧率下移动速度不一致**：
   - `velocity = movementSpeed * deltaTime;`
4. **Pitch 未限制导致“翻跟头”**：
   - 记得在 `processMouseMovement` 里开启 `constrainPitch` 并夹在 \[-89°, 89°]。
5. **忘记开启深度测试**：
   - `glEnable(GL_DEPTH_TEST);` 以及每帧 `glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);`。

---

### 6) 练习

- **练习 1**：把 `Camera` 改成“飞行模式”，允许沿世界 `up` 方向上升/下降（加入 `SPACE` / `LEFT_SHIFT` 控制）。
- **练习 2**：实现 `glfwSetScrollCallback`，把鼠标滚轮接到 `processMouseScroll` 上，体验“拉近 / 推远”视角。
- **练习 3**：给摄像机加一个“重置”功能，按 `R` 键恢复到初始位置和朝向。
- **练习 4**：尝试把 `cubePositions` 换成“长廊”或“迷宫”布局，走一走自己的 3D 小世界。

