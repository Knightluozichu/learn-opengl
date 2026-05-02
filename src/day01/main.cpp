#include <glad/glad.h> // GLAD: 负责按需加载 OpenGL 函数指针
#include <GLFW/glfw3.h> // GLFW: 负责创建窗口、上下文以及处理输入
#include <iostream> // 用于输出错误或调试信息

const char* vertexShaderSource = R"(#version 330 core
layout (location = 0) in vec3 aPos;
void main() {
    gl_Position = vec4(aPos, 1.0);
})"; // 顶点着色器源码：把传入的顶点位置直接输出到裁剪空间

const char* fragmentShaderSource = R"(#version 330 core
out vec4 FragColor;
void main() {
    FragColor = vec4(0.95, 0.6, 0.1, 1.0);
})"; // 片段着色器源码：给每个像素上橙黄色

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height); // 当窗口尺寸变化时，更新视口大小以避免拉伸
} // GLFW 要求的回调函数签名

int main() { // 程序入口
    if (!glfwInit()) { // 初始化 GLFW，失败则直接退出
        std::cerr << "Failed to init GLFW\n"; // 输出初始化失败原因
        return -1; // 用 -1 表示异常退出
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // 申请 OpenGL 主版本号 3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3); // 申请 OpenGL 次版本号 3，即 3.3
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // 使用核心模式，禁用旧版固定管线
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // macOS 需要这个标志来兼容核心上下文

    GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL", nullptr, nullptr); // 创建 800x600 的窗口
    if (!window) { // 判断窗口是否创建成功
        std::cerr << "Failed to create GLFW window\n"; // 提示创建失败
        glfwTerminate(); // 清理 GLFW 资源
        return -1; // 返回错误码
    }
    glfwMakeContextCurrent(window); // 将刚创建的窗口上下文切换为当前线程正在使用的上下文
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback); // 注册窗口尺寸变化的回调

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) { // 通过 GLAD 加载所有 OpenGL 函数地址
        std::cerr << "Failed to initialize GLAD\n"; // 如果失败，输出错误
        return -1; // 退出程序
    }

    float vertices[] = { // 一个简单三角形的 3 个顶点
        -0.5f, -0.5f, 0.0f, // 左下角
         0.0f,  0.5f, 0.0f, // 顶部
         0.5f, -0.5f, 0.0f // 右下角
    };

    unsigned int VBO, VAO; // VBO 用于存储顶点数据，VAO 记录顶点属性配置
    glGenVertexArrays(1, &VAO); // 生成 1 个 VAO ID
    glGenBuffers(1, &VBO); // 生成 1 个 VBO ID

    glBindVertexArray(VAO); // 绑定 VAO，使得后续配置与之关联
    glBindBuffer(GL_ARRAY_BUFFER, VBO); // 绑定 VBO 到 GL_ARRAY_BUFFER 目标
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); // 把顶点数据复制到显存，GL_STATIC_DRAW 表示几乎不改动

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0); // 告诉 OpenGL 顶点属性如何解析
    glEnableVertexAttribArray(0); // 启用位置 0 的属性，使着色器能读取数据

    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER); // 创建一个顶点着色器对象
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr); // 将源代码附加到顶点着色器
    glCompileShader(vertexShader); // 编译顶点着色器

    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER); // 创建一个片段着色器对象
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr); // 给片段着色器附加源代码
    glCompileShader(fragmentShader); // 编译片段着色器

    unsigned int shaderProgram = glCreateProgram(); // 创建一个着色器程序
    glAttachShader(shaderProgram, vertexShader); // 把顶点着色器附加到程序
    glAttachShader(shaderProgram, fragmentShader); // 把片段着色器附加到程序
    glLinkProgram(shaderProgram); // 链接程序，把两个着色器组合成可运行的 GPU 程序

    glDeleteShader(vertexShader); // 链接完成后就可以删除单独的着色器对象
    glDeleteShader(fragmentShader); // 同理删除片段着色器

    glClearColor(0.15f, 0.2f, 0.25f, 1.0f); // 设置清屏颜色，方便看到背景

    while (!glfwWindowShouldClose(window)) { // 主循环：直到窗口被要求关闭
        glClear(GL_COLOR_BUFFER_BIT); // 用之前设定的颜色清空颜色缓冲

        glUseProgram(shaderProgram); // 激活着色器程序，后续绘制都用它
        glBindVertexArray(VAO); // 绑定 VAO，告诉 GPU 使用哪套顶点数据
        glDrawArrays(GL_TRIANGLES, 0, 3); // 绘制 3 个顶点组成的三角形

        glfwSwapBuffers(window); // 交换前后缓冲，把绘制好的画面显示出来
        glfwPollEvents(); // 处理输入、窗口等事件
    }

    glDeleteVertexArrays(1, &VAO); // 释放 VAO 资源
    glDeleteBuffers(1, &VBO); // 释放 VBO 资源
    glDeleteProgram(shaderProgram); // 释放着色器程序
    glfwTerminate(); // 关闭 GLFW，清理所有资源
    return 0; // 主函数返回 0，说明正常结束
}