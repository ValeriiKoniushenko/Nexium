#version 430 core

uniform mat4 uProjAndView;

uniform vec3 uCameraPos;

const vec3 Pos[4] = vec3[4] (
    vec3(-100.0, 0.0,-100.0), // BL
    vec3( 100.0, 0.0,-100.0), // BR
    vec3( 100.0, 0.0, 100.0), // TR
    vec3(-100.0, 0.0, 100.0)  // TL
);

const int Indicies[6] = int[6](0,2,1,2,0,3);

out vec3 ioWorldPos;

void main()
{
    int index = Indicies[gl_VertexID];

    vec3 pos = Pos[index];
    pos.x += -uCameraPos.x;
    pos.z += -uCameraPos.z;

    ioWorldPos = pos;

    gl_Position = uProjAndView vec4(pos, 1.0);
}