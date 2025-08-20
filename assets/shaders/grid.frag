#version 430 core

out vec4 FragColor;

uniform float uGridCellSize = 0.025;
uniform vec4 uGridColorThin = vec4(0.5,0.5,0.5,1.0);
uniform vec4 uGridColorThick = vec4(0,0,0,1);

out vec3 ioWorldPos;

void main()
{
    float lod0a = mod(ioWorldPos.z, uGridCellSize);
    
    vec4 color;
    color = uGridColorThin;

    FragColor = color;
}