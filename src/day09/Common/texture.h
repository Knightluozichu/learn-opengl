#pragma once

#include <string>
using namespace std;

class Texture
{
public:
    Texture(const string& path);
    ~Texture();
    unsigned int ID;
    int width;
    int height;
    int channels;
    unsigned char* data;
    void bind(unsigned int slot = 0) const;
    void unbind() const;
    int getWidth() const;
    int getHeight() const;
    int getChannels() const;
};