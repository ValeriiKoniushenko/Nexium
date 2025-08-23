#version 460 core
out vec4 FragColor;

uniform vec4 uOutlineColor = vec4(1.0, 0.5, 0.0, 1.0);

void main()
{
    FragColor = uOutlineColor;
}