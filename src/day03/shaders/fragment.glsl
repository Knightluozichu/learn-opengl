#version 330 core
uniform vec3 uW;
out vec4 fragColor;
in vec3 ourColor;
in vec3 vertexPos;
void main()
{
    vec3 c1 = ourColor;
    vec3 c2 = vec3(ourColor.b, ourColor.r, ourColor.g);
    vec3 c3 = vec3(ourColor.g, ourColor.b, ourColor.r); 
    vec3 mixed = uW.x*c1 + uW.y*c2 + uW.z*c3;
    fragColor = vec4(mixed, 1.0);

    // fragColor = vec4(vertexPos, 1.0); 练习3
}