#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

// texture sampler
uniform sampler2D texture1;
uniform sampler2D texture2;

void main()
{
    // 简单的混合两个纹理 (0.2的权重给第二个)
    // FragColor = mix(texture(texture1, TexCoord), texture(texture2, TexCoord), 0.2);
    
    // Day06 简化版：只用一张图，或者你自己根据需要修改
    FragColor = texture(texture1, TexCoord);
}
