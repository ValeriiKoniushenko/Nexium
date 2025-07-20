#version 430 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

out vec3 ioFragPos;
out vec3 ioNormal;

uniform mat4 uProjAndView;
uniform mat4 uModel;

void main()
{
   ioFragPos = vec3(uModel * vec4(aPos, 1.0));
   ioNormal = mat3(transpose(inverse(uModel))) * aNormal;

   gl_Position = uProjAndView * uModel * vec4(aPos, 1.0);
}