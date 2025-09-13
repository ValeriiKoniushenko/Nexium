// Shader: color
#version 460 core

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