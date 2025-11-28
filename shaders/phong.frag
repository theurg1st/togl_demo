#version 330 core

out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 UV;

uniform vec3 lightPos;
uniform vec3 viewPos;

void main()
{
    vec3 color = vec3(0.2, 1.0, 0.3);   // green

    // ambient
    float ambientStrength = 0.15;
    vec3 ambient = ambientStrength * color;

    // diffuse
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * color;

    // specular
    float specStrength = 0.5;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);

    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 16);
    vec3 specular = specStrength * spec * vec3(1.0);

    vec3 result = ambient + diffuse + specular;

    FragColor = vec4(result, 1.0);
}
