#version 450 core

out vec4 FragColor;

in vec2 ioUV;

uniform sampler2D uTexture;
uniform bool uAlphaBlendingEnabled;

void main()
{
    vec4 texColor = texture(uTexture, ioUV);

    if (uAlphaBlendingEnabled && texColor.a < 0.1)
    {
        discard;
    }
    if (!uAlphaBlendingEnabled)
    {
        texColor.a = 1.0;
    }

    FragColor = texColor;
}
