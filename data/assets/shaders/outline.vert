// Shader: outline
#version 450 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

uniform mat4 uProjAndView;
uniform mat4 uModel;
uniform float uOutlineSize = 50.0;
uniform float uCameraObjectNDCDistance;

void main()
{
    vec3 normal = normalize(mat3(uProjAndView * uModel) * aNormal);
    vec3 offset = normal * uOutlineSize * uCameraObjectNDCDistance;

    vec4 pos = uProjAndView * uModel * vec4(aPos, 1.0);

    pos.xy += offset.xy;

    gl_Position = pos;
}