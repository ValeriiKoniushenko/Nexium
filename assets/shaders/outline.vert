#version 460 core

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec2 aUv;
layout (location = 2) in vec3 aNormal;
layout (location = 3) in vec2 aTextureRectSize;

uniform mat4 uModel;
uniform mat4 uProjectionAndView;

void main()
{
    gl_Position = uProjectionAndView * uModel * vec4(aPosition, 1.f);
}