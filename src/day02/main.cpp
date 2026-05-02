// // 复习 day01 内容=======================================================

// #include <glad/glad.h>
// #include <GLFW/glfw3.h>
// #include <iostream>

// void framebuffer_size_callback(GLFWwindow* window, int width, int height);
// void processInput(GLFWwindow *window);

// int main()
// {
//     glfwInit();
//     glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3);
//     glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,3);
//     glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);
//     glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT,GL_TRUE);
    
//     GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL",nullptr, nullptr);
//     if(NULL == window)
//     {
//         std::cout << "创建 glfw 窗口失败！" << std::endl;
//         glfwTerminate();
//         return -1;
//     }
//     glfwMakeContextCurrent(window);

//     if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
//     {
//         std::cout << "初始化 glad 失败！" <<std::endl;
//         return -1;
//     }

//     glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

//     float vertices[] ={
//         -0.5f, -0.5f, 0.0f,
//         0.5f, -0.5f, 0.0f,
//         0.0f, 0.5f, 0.0f
//     };

//     float verticesRed[] ={
//         0.0f, 0.5f, 0.0f,
//         0.5f, 0.5f, 0.0f,
//         0.5f, -0.5f,0.0f
//     };

//     unsigned int VBO;
//     glGenBuffers(1, &VBO);
//     glBindBuffer(GL_ARRAY_BUFFER, VBO);
//     glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

//     unsigned int VAO;
//     glGenVertexArrays(1, &VAO);
//     glBindVertexArray(VAO);

//     glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
//     glEnableVertexAttribArray(0);

//     unsigned int VBORed;
//     glGenBuffers(1, &VBORed);
//     glBindBuffer(GL_ARRAY_BUFFER, VBORed);
//     glBufferData(GL_ARRAY_BUFFER, sizeof(verticesRed), verticesRed, GL_STATIC_DRAW);

//     unsigned int VAORed;
//     glGenVertexArrays(1, &VAORed);
//     glBindVertexArray(VAORed);

//     glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
//     glEnableVertexAttribArray(0);

//     const char* vertexShaderSource = R"(
//     #version 330 core
//     layout (location = 0) in vec3 aPos;
//     void main()
//     {
//         gl_Position = vec4(aPos, 1.0);
//     })";

//     const char* fragmentShaderSource = R"(
//     #version 330 core
//     out vec4 FragColor;
//     void main()
//     {
//         FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);
//     })";

//     const char* fragmenthaderSourceRed = R"(
//     #version 330 core
//     out vec4 FragColor;
//     void main()
//     {
//         FragColor = vec4(1.0f, 0.0f, 0.0f, 0.5f);
//     }
//     )";

//     unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
//     glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
//     glCompileShader(vertexShader);

//     unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
//     glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
//     glCompileShader(fragmentShader);

//     unsigned int fragmentShaderRed = glCreateShader(GL_FRAGMENT_SHADER);
//     glShaderSource(fragmentShaderRed, 1, &fragmenthaderSourceRed, nullptr);
//     glCompileShader(fragmentShaderRed);

//     unsigned int shaderProgramRed = glCreateProgram();
//     glAttachShader(shaderProgramRed,vertexShader);
//     glAttachShader(shaderProgramRed, fragmentShaderRed);
//     glLinkProgram(shaderProgramRed);

//     unsigned int shaderProgram = glCreateProgram();
//     glAttachShader(shaderProgram,vertexShader);
//     glAttachShader(shaderProgram, fragmentShader);
//     glLinkProgram(shaderProgram);




//     while(!glfwWindowShouldClose(window))
//     {
//         processInput(window);

//         glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
//         glClear(GL_COLOR_BUFFER_BIT);

//         glUseProgram(shaderProgram);
//         glBindVertexArray(VAO);
//         glDrawArrays(GL_TRIANGLES, 0, 3);


//         glUseProgram(shaderProgramRed);
//         glBindVertexArray(VAORed);
//         glDrawArrays(GL_TRIANGLES, 0, 3);

//         glfwSwapBuffers(window);
//         glfwPollEvents();
//     }

//     glDeleteVertexArrays(1, &VAO);
//     glDeleteBuffers(1, &VBO);
//     glDeleteProgram(shaderProgram);
//     glDeleteShader(vertexShader);
//     glDeleteShader(fragmentShader);

//     glfwTerminate();
//     return 0;

// }

// void framebuffer_size_callback(GLFWwindow* window, int width, int height)
// {
//     glViewport(0,0,width,height);
// }

// void processInput(GLFWwindow *window)
// {
//     if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
//     {
//         glfwSetWindowShouldClose(window, true);
//     }
// }

//=================================================================================

// 添加更多顶点到数据中，使用glDrawArrays，尝试绘制两个彼此相连的三角形
//day02 练习 1 内容=================================================================
// #include <glad/glad.h>
// #include <GLFW/glfw3.h>

// #include <iostream>

// void framebuffer_size_callback(GLFWwindow* window, int width, int height);
// void processInput(GLFWwindow *window);

// // settings
// const unsigned int SCR_WIDTH = 800;
// const unsigned int SCR_HEIGHT = 600;

// const char *vertexShaderSource = "#version 330 core\n"
//     "layout (location = 0) in vec3 aPos;\n"
//     "void main()\n"
//     "{\n"
//     "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
//     "}\0";
// const char *fragmentShaderSource = "#version 330 core\n"
//     "out vec4 FragColor;\n"
//     "void main()\n"
//     "{\n"
//     "   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
//     "}\n\0";

// int main()
// {
//     // glfw: initialize and configure
//     // ------------------------------
//     glfwInit();
//     glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
//     glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
//     glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

// #ifdef __APPLE__
//     glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
// #endif

//     // glfw window creation
//     // --------------------
//     GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
//     if (window == NULL)
//     {
//         std::cout << "Failed to create GLFW window" << std::endl;
//         glfwTerminate();
//         return -1;
//     }
//     glfwMakeContextCurrent(window);
//     glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

//     // glad: load all OpenGL function pointers
//     // ---------------------------------------
//     if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
//     {
//         std::cout << "Failed to initialize GLAD" << std::endl;
//         return -1;
//     }


//     // build and compile our shader program
//     // ------------------------------------
//     // vertex shader
//     unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
//     glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
//     glCompileShader(vertexShader);
//     // check for shader compile errors
//     int success;
//     char infoLog[512];
//     glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
//     if (!success)
//     {
//         glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
//         std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
//     }
//     // fragment shader
//     unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
//     glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
//     glCompileShader(fragmentShader);
//     // check for shader compile errors
//     glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
//     if (!success)
//     {
//         glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
//         std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
//     }
//     // link shaders
//     unsigned int shaderProgram = glCreateProgram();
//     glAttachShader(shaderProgram, vertexShader);
//     glAttachShader(shaderProgram, fragmentShader);
//     glLinkProgram(shaderProgram);
//     // check for linking errors
//     glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
//     if (!success) {
//         glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
//         std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
//     }
//     glDeleteShader(vertexShader);
//     glDeleteShader(fragmentShader);

//     // set up vertex data (and buffer(s)) and configure vertex attributes
//     // ------------------------------------------------------------------
//     // add a new set of vertices to form a second triangle (a total of 6 vertices); the vertex attribute configuration remains the same (still one 3-float position vector per vertex)
//     float vertices[] = {
//         // first triangle
//         -0.9f, -0.5f, 0.0f,  // left 
//         -0.0f, -0.5f, 0.0f,  // right
//         -0.45f, 0.5f, 0.0f,  // top 
//         // second triangle
//          0.0f, -0.5f, 0.0f,  // left
//          0.9f, -0.5f, 0.0f,  // right
//          0.45f, 0.5f, 0.0f   // top 
//     }; 

//     unsigned int VBO, VAO;
//     glGenVertexArrays(1, &VAO);
//     glGenBuffers(1, &VBO);
//     // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
//     glBindVertexArray(VAO);

//     glBindBuffer(GL_ARRAY_BUFFER, VBO);
//     glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

//     glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
//     glEnableVertexAttribArray(0);

//     // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
//     glBindBuffer(GL_ARRAY_BUFFER, 0); 

//     // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
//     // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
//     glBindVertexArray(0); 


//     // uncomment this call to draw in wireframe polygons.
//     //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

//     // render loop
//     // -----------
//     while (!glfwWindowShouldClose(window))
//     {
//         // input
//         // -----
//         processInput(window);

//         // render
//         // ------
//         glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
//         glClear(GL_COLOR_BUFFER_BIT);

//         // draw our first triangle
//         glUseProgram(shaderProgram);
//         glBindVertexArray(VAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
//         glDrawArrays(GL_TRIANGLES, 0, 6); // set the count to 6 since we're drawing 6 vertices now (2 triangles); not 3!
//         // glBindVertexArray(0); // no need to unbind it every time 
 
//         // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
//         // -------------------------------------------------------------------------------
//         glfwSwapBuffers(window);
//         glfwPollEvents();
//     }

//     // optional: de-allocate all resources once they've outlived their purpose:
//     // ------------------------------------------------------------------------
//     glDeleteVertexArrays(1, &VAO);
//     glDeleteBuffers(1, &VBO);
//     glDeleteProgram(shaderProgram);

//     // glfw: terminate, clearing all previously allocated GLFW resources.
//     // ------------------------------------------------------------------
//     glfwTerminate();
//     return 0;
// }

// // process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// // ---------------------------------------------------------------------------------------------------------
// void processInput(GLFWwindow *window)
// {
//     if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
//         glfwSetWindowShouldClose(window, true);
// }

// // glfw: whenever the window size changed (by OS or user resize) this callback function executes
// // ---------------------------------------------------------------------------------------------
// void framebuffer_size_callback(GLFWwindow* window, int width, int height)
// {
//     // make sure the viewport matches the new window dimensions; note that width and 
//     // height will be significantly larger than specified on retina displays.
//     glViewport(0, 0, width, height);
// }

//=================================================================================

// 创建相同的两个三角形，但对它们的数据使用不同的VAO和VBO：
//day02 练习 2 内容=================================================================

// #include <glad/glad.h>
// #include <GLFW/glfw3.h>

// #include <iostream>
// void framebuffer_size_callback(GLFWwindow* window, int width, int height);
// void processInput(GLFWwindow *window);

// const unsigned int SCR_WIDTH = 800;
// const unsigned int SCR_HEIGHT = 600;

// const char *vertexShaderSource = "#version 330 core\n"
// "layout (location = 0) in vec3 aPos;\n"
// "void main()\n"
// "{\n"
// "gl_Position = vec4(aPos, 1.0);\n"
// "}\0";

// const char *fragmentShaderSource = "#version 330 core\n"
// "out vec4 FragColor;\n"
// "void main()\n"
// "{\n"
// "FragColor = vec4(1.0f, 1.0f, 0.0f, 1.0f);\n"
// "}\0";

// int main()
// {
//     glfwInit();
//     glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
//     glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,3);
//     glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);
    
//     #if __APPLE__
//     glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
//     #endif

//     GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Opengl Practice 2", nullptr, nullptr );
//     if(NULL == window)
//     {
//         std::cout << "打开窗口失败!" << std::endl;
//         glfwTerminate();
//         return -1;
//     }

//     glfwMakeContextCurrent(window);
//     glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

//     if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
//     {
//         std::cout << "初始化 glad 失败!" << std::endl;
//         return -1;
//     }

//     //创建编译着色器
//     unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
//     glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
//     glCompileShader(vertexShader);

//     int success;
//     char infoLog[512];
//     glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
//     if(!success)
//     {
//         glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
//         std::cout << "顶点着色器编译失败: " << infoLog << std::endl;
//     }

//     unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
//     glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
//     glCompileShader(fragmentShader);

//     glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
//     if(!success)
//     {
//         glGetShaderInfoLog(fragmentShader, 512, nullptr, infoLog);
//         std::cout << "片段着色器编译失败: " << infoLog << std::endl;
//     }

//     unsigned int shaderProgram = glCreateProgram();
//     glAttachShader(shaderProgram, vertexShader);
//     glAttachShader(shaderProgram, fragmentShader);
//     glLinkProgram(shaderProgram);

//     glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
//     if(!success)
//     {
//         glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
//         std::cout << "着色器程序链接失败: " << infoLog << std::endl;
//     }

//     glDeleteShader(vertexShader);
//     glDeleteShader(fragmentShader);

//     float firstTriangle[] = {
//         -0.9f, -0.5f, 0.0f,
//         -0.0f, -0.5f, 0.0f,
//         -0.45f, 0.5f, 0.0f,
//     };
//     float secondTriangle[] = {
//         0.0f, -0.5f, 0.0f,
//         0.9f, -0.5f, 0.0f,
//         0.45f, 0.5f, 0.0f,
//     };

//     unsigned int VBOs[2], VAOs[2];
//     glGenVertexArrays(2, VAOs);
//     glGenBuffers(2, VBOs);

//     glBindVertexArray(VAOs[0]);
//     glBindBuffer(GL_ARRAY_BUFFER,VBOs[0]);
//     glBufferData(GL_ARRAY_BUFFER, sizeof(firstTriangle), firstTriangle, GL_STATIC_DRAW);
//     glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,3*sizeof(float),NULL);
//     glEnableVertexAttribArray(0);

//     glBindVertexArray(VAOs[1]);
//     glBindBuffer(GL_ARRAY_BUFFER, VBOs[1]);
//     glBufferData(GL_ARRAY_BUFFER,sizeof(secondTriangle), secondTriangle, GL_STATIC_DRAW);
//     glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE, 3*sizeof(float), NULL);
//     glEnableVertexAttribArray(0);

//     while(!glfwWindowShouldClose(window))
//     {
//         processInput(window);
//         glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
//         glClear(GL_COLOR_BUFFER_BIT);

//         glUseProgram(shaderProgram);
//         glBindVertexArray(VAOs[0]);
//         glDrawArrays(GL_TRIANGLES, 0, 3);

//         glBindVertexArray(VAOs[1]);
//         glDrawArrays(GL_TRIANGLES, 0, 3);

//         glfwSwapBuffers(window);
//         glfwPollEvents();
//     }

//     glDeleteVertexArrays(2, VAOs);
//     glDeleteBuffers(2, VBOs);
//     glDeleteProgram(shaderProgram);

//     glfwTerminate();
//     return 0;
// }


// void processInput(GLFWwindow *window)
// {
//     if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
//     {
//         glfwSetWindowShouldClose(window, true);
//     }
// }

// void framebuffer_size_callback(GLFWwindow* window, int width, int height)
// {
//     glViewport(0,0,width,height);
// }

//=================================================================================

// 创建两个着色器程序，第二个程序使用一个不同的片段着色器，输出橙色；再次绘制这两个
// 三角形，让其中一个输出为黄色
//day02 练习 3 内容=================================================================
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

using namespace std;

void processInput(GLFWwindow *window);
void framebuffer_size_callback(GLFWwindow *window, int width, int height);

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

const char *vertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
void main()
{
    gl_Position = vec4(aPos, 1.0f);
}
)";

const char *fragmentShaderSourceOrange = R"(
#version 330 core
out vec4 fragColor;
void main()
{
    fragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);
}
)";

const char *fragmentShaderSourceYellow = R"(
#version 330 core
out vec4 fragColor;
void main()
{
    fragColor = vec4(1.0f,1.0f,0.0f,1.0f);
}
)";

int main()
{
    // 初始化及配置 glfw
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    #if __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #endif

    // 创建窗口
    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "opengl practicle 3", nullptr , nullptr);
    if(NULL == window)
    {
        cout << "创建窗口失败!" << endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // 初始化 glad ， 加载所有 OpenGL 函数指针
    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        cout << "glad初始化失败!" <<endl;
        return -1;
    }
    cout << "opengl 版本:" << glGetString(GL_VERSION) << endl;
    //  创建顶点着色器
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    glCompileShader(vertexShader);
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
        cout << "顶点着色器编译失败: " << infoLog << endl;
        return -1;
    }

    //  创建橙色片段着色器
    unsigned int fragmentShaderOrange = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShaderOrange, 1, &fragmentShaderSourceOrange, nullptr);
    glCompileShader(fragmentShaderOrange);
    glGetShaderiv(fragmentShaderOrange, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        glGetShaderInfoLog(fragmentShaderOrange, 512, nullptr, infoLog);
        cout << "橙色片段着色器编译失败: " << infoLog << endl;
        return -1;
    }

    //  创建黄色片段着色器
    unsigned int fragmentShaderYellow = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShaderYellow, 1, &fragmentShaderSourceYellow, nullptr);
    glCompileShader(fragmentShaderYellow);
    glGetShaderiv(fragmentShaderYellow, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        glGetShaderInfoLog(fragmentShaderYellow, 512, nullptr, infoLog);
        cout << "黄色片段着色器编译失败: " << infoLog << endl;
        return -1;
    }

    //  创建橙色着色器程序
    unsigned int shaderProgramOrange = glCreateProgram();
    glAttachShader(shaderProgramOrange, vertexShader);
    glAttachShader(shaderProgramOrange, fragmentShaderOrange);
    glLinkProgram(shaderProgramOrange);
    glGetProgramiv(shaderProgramOrange, GL_LINK_STATUS, &success);
    if(!success)
    {
        glGetProgramInfoLog(shaderProgramOrange, 512, nullptr, infoLog);
        cout << "橙色着色器程序链接失败: " << infoLog << endl;
        return -1;
    }

    //  创建黄色着色器程序
    unsigned int shaderProgramYellow = glCreateProgram();
    glAttachShader(shaderProgramYellow, vertexShader);
    glAttachShader(shaderProgramYellow, fragmentShaderYellow);
    glLinkProgram(shaderProgramYellow);
    glGetProgramiv(shaderProgramYellow, GL_LINK_STATUS, &success);
    if(!success)
    {
        glGetProgramInfoLog(shaderProgramYellow, 512, nullptr, infoLog);
        cout << "黄色着色器程序链接失败: " << infoLog << endl;
        return -1;
    }

    //  删除不再需要的着色器
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShaderOrange);
    glDeleteShader(fragmentShaderYellow);

    //  设置顶点数据
    // float firstTriangle[] = {
    //     -0.9f, -0.5f, 0.0f,
    //     -0.0f, -0.5f, 0.0f,
    //     -0.45f, 0.5f, 0.0f,
    // };
    // float secondTriangle[] = {
    //     0.0f, -0.5f, 0.0f,
    //     0.9f, -0.5f, 0.0f,
    //     0.45f, 0.5f, 0.0f,
    // };

    float firstVertices[] = {
        -0.9f, -0.5f, 0.0f,   // 索引 0: 左下
        -0.0f, -0.5f, 0.0f,   // 索引 1: 右下
        -0.45f, 0.5f, 0.0f,   // 索引 2: 顶部
    };
    unsigned int firstIndices[] = {
        0, 1, 2   // 第一个三角形
    };
    float secondVertices[] = {
        0.0f, -0.5f, 0.0f,   // 索引 0: 左下
        0.9f, -0.5f, 0.0f,   // 索引 1: 右下
        0.45f, 0.5f, 0.0f,   // 索引 2: 顶部
    };
    unsigned int secondIndices[] = {
        0, 1, 2   // 第二个三角形
    };

    unsigned int VBOs[2], VAOs[2], EBOs[2];
    glGenVertexArrays(2, VAOs);
    glGenBuffers(2, VBOs);
    glGenBuffers(2, EBOs);

    glBindVertexArray(VAOs[0]);
    glBindBuffer(GL_ARRAY_BUFFER, VBOs[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(firstVertices), firstVertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOs[0]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(firstIndices), firstIndices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);

    glBindVertexArray(VAOs[1]);
    glBindBuffer(GL_ARRAY_BUFFER, VBOs[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(secondVertices), secondVertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOs[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(secondIndices), secondIndices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);

    while(!glfwWindowShouldClose(window))
    {
        processInput(window);
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgramOrange);
        glBindVertexArray(VAOs[0]);
        // glDrawArrays(GL_TRIANGLES, 0, 3);
        glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);

        glUseProgram(shaderProgramYellow);
        glBindVertexArray(VAOs[1]);
        // glDrawArrays(GL_TRIANGLES, 0, 3);
        glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(2, VAOs);
    glDeleteBuffers(2, VBOs);
    glDeleteBuffers(2, EBOs);
    glDeleteProgram(shaderProgramOrange);
    glDeleteProgram(shaderProgramYellow);

    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow *window)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    glViewport(0,0,width, height);
}