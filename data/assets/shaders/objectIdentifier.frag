// Shader: objectIdentifier
#version 450 core

out vec4 FragColor;
uniform vec3 uPickingColor = vec3(0.0, 0.0, 0.0);

void main()
{
    FragColor = vec4(uPickingColor, 1.0);
}
