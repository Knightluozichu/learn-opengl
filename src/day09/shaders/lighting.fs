#version 330 core
out vec4 FragColor;
in vec3 Normal;
in vec3 FragPos;
in vec3 LightPos;

uniform vec3 objectColor;
uniform vec3 lightColor;
uniform float amibientStrength;

uniform vec3 viewPos;


void main()
{
    vec3 ambientColor = objectColor * amibientStrength;

    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(LightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuseColor = diff * lightColor;

    // vec3 result = (diffuseColor + ambientColor) * objectColor;
    // FragColor = vec4(diff * 0.5f + 0.5f, 1.0);
    // float d = dot(normalize(Normal), normalize(LightPos - FragPos));
    // FragColor = vec4(max(d, 0.0), max(-d, 0.0), 0.0, 1.0);

    vec3 result = (diffuseColor + ambientColor) * objectColor;
    FragColor = vec4(result, 1.0);
}
