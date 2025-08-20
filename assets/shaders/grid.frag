#version 430 core

out vec4 FragColor;

uniform float uGridCellSize = 0.1;
uniform vec4 uGridColorThin = vec4(0.5, 0.5, 0.5, 1.0);
uniform vec4 uGridColorThick = vec4(0.0, 0.0, 0.0, 1.0);

in vec3 ioWorldPos;

void main()
{
    float alpha = mod(ioWorldPos.z, uGridCellSize);
    
    vec4 color = uGridColorThick;
    color.a *= alpha;

    FragColor = color;
}