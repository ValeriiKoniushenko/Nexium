#version 450 core
layout(location = 0) in vec4 aVertex; // <vec2 pos, vec2 tex>

out vec2 ioTexCoords;

uniform mat4 uProj;
uniform vec2 uPos;

void main()
{
    mat4 model = mat4(1);
    model[3][0] = uPos.x;
    model[3][1] = uPos.y;

    gl_Position = uProj * model * vec4(aVertex.xy, 0.0, 1.0);
    ioTexCoords = aVertex.zw;
}