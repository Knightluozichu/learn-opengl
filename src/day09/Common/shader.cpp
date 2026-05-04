#include "shader.h"
#include <cerrno>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>

using namespace std;

static bool readShaderFile(const char* path, const char* tag, string& out)
{
    ifstream f;
    f.exceptions(ifstream::failbit | ifstream::badbit);
    try
    {
        f.open(path);
        stringstream ss;
        ss << f.rdbuf();
        f.close();
        out = ss.str();
        if (out.empty())
        {
            cerr << "ERROR::SHADER::FILE_EMPTY [" << tag << "] path=" << path << endl;
            return false;
        }
        return true;
    }
    catch (ifstream::failure& e)
    {
        namespace fs = std::filesystem;
        std::error_code ec;
        bool exists = fs::exists(path, ec);
        cerr << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ [" << tag << "]\n"
             << "  path   : " << path << "\n"
             << "  abspath: " << fs::absolute(path, ec).string() << "\n"
             << "  exists : " << (exists ? "yes" : "NO") << "\n"
             << "  cwd    : " << fs::current_path(ec).string() << "\n"
             << "  errno  : " << errno << " (" << std::strerror(errno) << ")\n"
             << "  what() : " << e.what() << endl;
        return false;
    }
}

Shader::Shader(const char* vertexPath, const char* fragmentPath)
    : ID(0)
{
    string vertexCode;
    string fragmentCode;
    if (!readShaderFile(vertexPath, "VERTEX", vertexCode)) return;
    if (!readShaderFile(fragmentPath, "FRAGMENT", fragmentCode)) return;

    const char* vShaderCode = vertexCode.c_str();
    const char* fShaderCode = fragmentCode.c_str();

    unsigned int vertex, fragment;

    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex,1,&vShaderCode,NULL);
    glCompileShader(vertex);
    checkCompileErrors(vertex, "VERTEX", vertexPath);

    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, NULL);
    glCompileShader(fragment);
    checkCompileErrors(fragment, "FRAGMENT", fragmentPath);

    ID = glCreateProgram();
    glAttachShader(ID, vertex);
    glAttachShader(ID, fragment);
    glLinkProgram(ID);
    checkCompileErrors(ID, "PROGRAM", "Shader Program");

    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

void Shader::use() const{
    glUseProgram(ID);
}

void Shader::setBool(const string &name,bool value) const{
    glUniform1i(glGetUniformLocation(ID, name.c_str()),(int)value);
}

void Shader::setInt(const string &name,int value) const{
    glUniform1i(glGetUniformLocation(ID, name.c_str()),value);
}

void Shader::setFloat(const string &name,float value) const{
    glUniform1f(glGetUniformLocation(ID, name.c_str()),value);
}

void Shader::setVec3(const string &name,float x,float y,float z) const{
    glUniform3f(glGetUniformLocation(ID, name.c_str()),x,y,z);
}

void Shader::setVec4(const string &name,float x,float y,float z,float w) const{
    glUniform4f(glGetUniformLocation(ID, name.c_str()),x,y,z,w);
}

void Shader::setMat4(const string &name,const float *value) const{
    glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()),1,GL_FALSE,value);
}

void Shader::checkCompileErrors(unsigned int shader,string type,string name){
    int success;
    char infoLog[1024];
    if(type != "PROGRAM")
    {
        glGetShaderiv(shader,GL_COMPILE_STATUS,&success);
        if(!success)
        {
            glGetShaderInfoLog(shader,1024,NULL,infoLog);
            cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << " in shader: " << name << endl;
            cout << infoLog << endl;
        }
    }
    else
    {
        glGetProgramiv(shader,GL_LINK_STATUS,&success);
        if(!success)
        {
            glGetProgramInfoLog(shader,1024,NULL,infoLog);
            cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << " in shader: " << name << endl;
            cout << infoLog << endl;
        }
    }
}
