// Shader: color
#version 450 core

// TODO: try to remvoe it
layout(std140, binding = 0) uniform Light {
    float ambientStrength;
    float specularStrength;
    float minLightStrength;
    float specularPow;

    vec3 color;
    vec3 lightPos;
    vec3 viewPos;
};

out vec4 FragColor;

in vec2 ioUV;

uniform sampler2D uTexture;

void main()
{
    vec4 texColor = texture(uTexture, ioUV);
    if (texColor.a < 0.1)
    {
        discard;
    }

    FragColor = texColor;
}