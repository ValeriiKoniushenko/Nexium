#version 430 core

const vec3 Pos[4] = vec3[4] (
    vec3(-1.0,-1.0, 0.0), // BL
    vec3( 1.0,-1.0, 0.0), // BR
    vec3( 1.0, 1.0, 0.0), // TR
    vec3(-1.0, 1.0, 0.0)  // TL
);

const int Indicies[6] = int[6](0,2,1,2,0,3);

void main()
{
    int index = Indicies[gl_VertexID];

    gl_Position = vec4(Pos[index], 1.0);
    //gl_Position = uProjAndView * vec4(aPos, 1.0);
}