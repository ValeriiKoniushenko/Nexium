#version 450 core

out vec4 FragColor;

in vec3 ioFragPos;
in vec3 ioNormal;
in vec2 ioUV;

layout(std140, binding = 0) uniform Light {
    float ambientStrength;
    float specularStrength;
    float minLightStrength;
    float specularPow;

    vec3 color;
    vec3 lightDir;
    vec3 viewPos;
};

uniform sampler2D uTexture;

void main()
{
    // ambient
    vec3 ambient = ambientStrength * color;

    // diffuse
    vec3 norm = normalize(ioNormal);
    vec3 viewDir = normalize(viewPos - ioFragPos);
    float diffuse = max(dot(norm, lightDir), minLightStrength);

    // specular
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), specularPow);
    float specular = specularStrength * spec;

    vec3 result = ambient * diffuse + specular;

    vec4 texColor = texture(uTexture, ioUV) * vec4(result, 1.0);
    if (texColor.a < 0.1)
    {
        discard;
    }

    FragColor = texColor;
}