#version 330 core
out vec4 FragColor;

uniform vec3 sinTime;

void main()
{

    FragColor =  vec4(sinTime,1.0); // 光源立方体永远是亮的白色
}
