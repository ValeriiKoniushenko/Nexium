#version 460 core
layout (location = 0) in vec3 aPos;

out vec3 ioTexCoords;

uniform mat4 uProj;
uniform mat4 uView;

void main()
{
    ioTexCoords = aPos;
    gl_Position = (uProj * uView * vec4(aPos, 1.0)).xyww;
}
