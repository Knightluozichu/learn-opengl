#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>
#include <string>

class Shader
{
public:
    // 程序 ID（需要在外部拿它来设置 uniform 或绑定）
    unsigned int ID;

    // 构造器：从文件读取并编译/链接着色器
    Shader(const char* vertexPath, const char* fragmentPath);

    // 使用/激活 着色器程序
    void use() const;

    // uniform 工具函数（最小集合：够 day03 用）
    void setBool(const std::string &name, bool value) const;
    void setInt(const std::string &name, int value) const;
    void setFloat(const std::string &name, float value) const;
    void setVec3(const std::string &name, float x, float y, float z) const;
};
#endif