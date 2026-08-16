// Shader: grid2d_simple
#version 460 core

uniform mat4 uProjAndView;
uniform vec2 uCameraPos;
uniform float uGlobalGridSize = 10000.0;

const vec2 Pos[4] = vec2[4](
vec2(-1.0, -1.0), // BL
vec2(1.0, -1.0), // BR
vec2(1.0, 1.0), // TR
vec2(-1.0, 1.0)  // TL
);

const int Indicies[6] = int[6](0, 2, 1, 2, 0, 3);

out vec2 ioWorldPos;

void main()
{
    int index = Indicies[gl_VertexID];

    vec2 pos = Pos[index] * uGlobalGridSize;
    pos.x += -uCameraPos.x;
    pos.y += -uCameraPos.y;

    ioWorldPos = pos;

    gl_Position = uProjAndView * vec4(pos, -1.0, 1.0);
}