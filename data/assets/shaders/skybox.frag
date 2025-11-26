#version 460 core
out vec4 oFragColor;

in vec3 ioTexCoords;

uniform samplerCube uSkybox;

void main()
{
    oFragColor = texture(uSkybox, ioTexCoords);
}
