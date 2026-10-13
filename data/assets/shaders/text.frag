#version 450 core

in vec2 ioTexCoords;
out vec4 color;

uniform sampler2D uTexture;
uniform vec3 uTextureColor = vec3(255, 0, 128);

void main()
{
    vec4 sampled = vec4(1.0, 1.0, 1.0, texture(uTexture, ioTexCoords).r);
    color = vec4(uTextureColor, 1.0) * sampled;
}