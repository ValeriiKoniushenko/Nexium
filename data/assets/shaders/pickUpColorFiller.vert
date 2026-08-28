// Shader: color
#version 450 core

layout (location = 0) in vec3 aPos;
layout (location = 2) in vec2 aUV;

out vec2 ioUV;

uniform mat4 uProjAndView;
uniform mat4 uModel;

void main()
{
    ioUV = vec2(aUV.x, aUV.y);

    gl_Position = uProjAndView * uModel * vec4(aPos, 1.0);
}