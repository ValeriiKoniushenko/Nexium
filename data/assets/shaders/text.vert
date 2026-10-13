#version 450 core
layout(location = 0) in vec4 aVertex; // <vec2 pos, vec2 tex>

out vec2 ioTexCoords;

uniform mat4 uProj;

void main()
{
    gl_Position = uProj * vec4(aVertex.xy, 0.0, 1.0);
    ioTexCoords = aVertex.zw;
}