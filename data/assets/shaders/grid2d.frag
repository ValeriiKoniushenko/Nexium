// Shader: grid2d_simple
#version 460 core

out vec4 FragColor;

uniform float uGridCellSize = 100.0;
uniform float uLineThickness = 0.85;
uniform float uSubLineThickness = 0.7;
uniform float uAxisLineThickness = 1.2;

uniform vec4 uGridColor = vec4(0.5, 0.5, 0.5, 0.5);
uniform vec4 uSubGridColor = vec4(0.7, 0.7, 0.7, 0.2);
uniform vec4 uAxisColorX = vec4(1.0, 0.2, 0.2, 1.0); // red — X axis
uniform vec4 uAxisColorY = vec4(0.2, 0.4, 1.0, 1.0); // blue — Y axis

in vec2 ioWorldPos;

void main()
{
    // Position within the current major cell, in [0, uGridCellSize)
    vec2 cellPos = mod(ioWorldPos, uGridCellSize);

    // --- Sub grid: single line through the center of each major cell ---
    // Distance from cellPos to the midline (uGridCellSize * 0.5) on each axis
    vec2 distToSubLine = abs(cellPos - uGridCellSize * 0.5);
    float subLine = min(distToSubLine.x, distToSubLine.y);

    vec4 color = vec4(0.0);
    if (subLine <= uSubLineThickness)
    {
        color = uSubGridColor;
    }

    // --- Major grid lines ---
    vec2 distToMajorLine = min(cellPos, uGridCellSize - cellPos);
    float majorLine = min(distToMajorLine.x, distToMajorLine.y);
    if (majorLine <= uLineThickness)
    {
        color = uGridColor;
    }

    // --- Axis lines (bold, colored, drawn on top) ---
    float distToXAxis = abs(ioWorldPos.y);
    float distToYAxis = abs(ioWorldPos.x);

    bool onXAxis = distToXAxis <= uAxisLineThickness;
    bool onYAxis = distToYAxis <= uAxisLineThickness;

    if (onXAxis)
    {
        color = uAxisColorX;
    }
    else if (onYAxis)
    {
        color = uAxisColorY;
    }

    if (color.a < 0.01)
    discard;

    FragColor = color;
}