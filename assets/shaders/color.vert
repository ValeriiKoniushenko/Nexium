// Shader: color
#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aUV;

out vec3 ioFragPos;
out vec3 ioNormal;
out vec2 ioUV;

uniform mat4 uProjAndView;
uniform mat4 uModel;

void main()
{
   ioFragPos = vec3(uModel * vec4(aPos, 1.0));
   ioNormal = mat3(transpose(inverse(uModel))) * aNormal;
   ioUV = vec2(aUV.x, aUV.y);

   gl_Position = uProjAndView * uModel * vec4(aPos, 1.0);
}