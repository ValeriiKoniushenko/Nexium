#version 460 core

layout(location = 0) in vec3 aPos;

out vec2 ioUV;

uniform mat4 uProjAndView;
uniform mat4 uModel;

uniform vec2 uUVOffset;
uniform vec2 uUVSize;

// clang-format off
const vec2 corners[4] = vec2[4](
    vec2(0.0, 1.0),
    vec2(0.0, 0.0),
    vec2(1.0, 0.0),
    vec2(1.0, 1.0)
);
// clang-format on

void main()
{
    vec2 uvSize = uUVSize;

    ioUV = uUVOffset + corners[gl_VertexID % 4] * uvSize;
    gl_Position = uProjAndView * uModel * vec4(aPos, 1.0);
}