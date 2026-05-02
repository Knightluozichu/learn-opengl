#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>
#include <string>

class Shader
{
public:
    // 程序 ID (需要在外部拿它来设置uniform 或绑定)
    unsigned int ID;
    // 构造器：从文件中读取并编译链接着色器
    Shader(const char *vertexPath, const char *fragmentPath);
    // 使用/激活 着色器程序
    void use() const;
    // uniform 工具函数
    void setBool(const std::string &name, bool value) const;
    void setInt(const std::string &name, int value) const;
    void setFloat(const std::string &name, float value) const;
    void setVec3(const std::string &name, float x, float y, float z) const;
    void setVec4(const std::string &name, float x, float y, float z, float w) const;
    void setMat4(const std::string &name, const float *value) const;

private:
    // 检查编译/链接错误
    void checkCompileErrors(unsigned int shader, std::string type);
};

#endif
