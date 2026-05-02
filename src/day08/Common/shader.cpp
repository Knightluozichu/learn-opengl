#include "shader.h"
#include <fstream>
#include <iostream>
#include <sstream>

using namespace std;

Shader::Shader(const char* vertexPath, const char* fragmentPath)
{
    string vertexCode;
    string fragmentCode;
    ifstream vShaderFile;
    ifstream fShaderFile;

    vShaderFile.exceptions(ifstream::failbit | ifstream::badbit);
    fShaderFile.exceptions(ifstream::failbit | ifstream::badbit);
    try
    {
        vShaderFile.open(vertexPath);
        fShaderFile.open(fragmentPath);

        stringstream vShaderStream, fShaderStream;
        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();

        vShaderFile.close();
        fShaderFile.close();
        vertexCode = vShaderStream.str();
        fragmentCode = fShaderStream.str();
        
    }catch(ifstream::failure& e)
    {
        cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ" << endl;
    }
    const char* vShaderCode = vertexCode.c_str();
    const char* fShaderCode = fragmentCode.c_str();

    unsigned int vertex, fragment;

    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex,1,&vShaderCode,NULL);
    glCompileShader(vertex);
    checkCompileErrors(vertex, "VERTEX");

    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, NULL);
    glCompileShader(fragment);
    checkCompileErrors(fragment, "FRAGMENT");

    ID = glCreateProgram();
    glAttachShader(ID, vertex);
    glAttachShader(ID, fragment);
    glLinkProgram(ID);
    checkCompileErrors(fragment, "PROGRAM");

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

void Shader::checkCompileErrors(unsigned int shader,string type){
    int success;
    char infoLog[1024];
    if(type != "PROGRAM")
    {
        glGetShaderiv(shader,GL_COMPILE_STATUS,&success);
        if(!success)
        {
            glGetShaderInfoLog(shader,1024,NULL,infoLog);
            cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << endl;
            cout << infoLog << endl;
        }
    }
    else
    {
        glGetProgramiv(shader,GL_LINK_STATUS,&success);
        if(!success)
        {
            glGetProgramInfoLog(shader,1024,NULL,infoLog);
            cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << endl;
            cout << infoLog << endl;
        }
    }
}
