// Shader: objectIdentifier
#version 450 core

layout (location = 0) in vec3 aPos;

uniform mat4 uProjAndView;
uniform mat4 uModel;

void main()
{
    gl_Position = uProjAndView * uModel * vec4(aPos, 1.0);
}