#version 330 core

in vec3 ourColor;
in vec2 TexCoord;

out vec4 fragColor;

uniform sampler2D ourTexture;
uniform sampler2D ourTexture2;

void main()
{
    vec4 texture1 = texture(ourTexture,TexCoord );// /256.0
    vec4 texture2 = texture(ourTexture2,vec2(1.0-TexCoord.x,TexCoord.y)); // 翻转笑脸
    fragColor = mix(texture1, texture2, 0.2); // * vec4(ourColor, 1.0) mix(texture1, texture2, 0.2)
}