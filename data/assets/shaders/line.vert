#version 330 core

layout(location = 0) in vec3 aPosition;

uniform mat4 uProjAndView;

void main()
{
    gl_Position = uProjAndView * vec4(aPosition, 1.0);
}