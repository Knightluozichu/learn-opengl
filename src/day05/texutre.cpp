#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <glad/glad.h>     // glad 必须在 GLFW 之前包含（避免 gl.h 先被引入）
#include <GLFW/glfw3.h>

#include "texture.h"
#include <iostream>
using namespace std;

Texture::Texture(const string& path)
{
    // LearnOpenGL 纹理坐标以左下为原点，图片通常以左上为原点，先翻转更直觉
    stbi_set_flip_vertically_on_load(true);
    data = stbi_load(path.c_str(), &width, &height, &channels, 0);
    if(data)
    {
        glGenTextures(1, &ID);
        glBindTexture(GL_TEXTURE_2D, ID);

        GLenum format = GL_RGB;
        if (channels == 1) format = GL_RED;
        else if (channels == 3) format = GL_RGB;
        else if (channels == 4) format = GL_RGBA;

        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
    else
    {
        cout << "Failed to load texture: " << path << " reason: " << (stbi_failure_reason() ? stbi_failure_reason() : "unknown") << endl;
        ID = 0;
        width = height = channels = 0;
    }
}

Texture::~Texture()
{
    stbi_image_free(data);
}

void Texture::bind(unsigned int slot) const
{
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, ID);
}

void Texture::unbind() const
{
    glBindTexture(GL_TEXTURE_2D, 0);
}

int Texture::getWidth() const
{
    return width;
}

int Texture::getHeight() const
{
    return height;
}

int Texture::getChannels() const
{
    return channels;
}
