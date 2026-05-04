#version 330 core
out vec4 FragColor;

uniform vec3 sinTime;
uniform float amibientStrength;
uniform vec3 lightColor;

void main()
{

    // FragColor =  vec4(sinTime,1.0); 
    FragColor = vec4(lightColor, 1.0);
}
