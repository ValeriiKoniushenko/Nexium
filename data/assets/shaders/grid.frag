#version 460 core

out vec4 FragColor;

uniform float uGlobalGridSize = 10000.0;
uniform float uMinimalDistanceBetweenPixels = 2.5;
uniform float uGridCellSize = 100.0;
uniform vec4 uGridColorThin = vec4(0.5, 0.5, 0.5, 0.5);
uniform vec4 uGridColorThick = vec4(0.0, 0.0, 0.0, 0.0);

// NEW
uniform vec4 uAxisColorX = vec4(1.0, 0.0, 0.0, 1.0); // red
uniform vec4 uAxisColorY = vec4(0.0, 0.0, 1.0, 1.0); // blue
uniform float uAxisLineWidth = 2.0; // in pixels, roughly

in vec3 ioWorldPos;
in vec2 ioPlanePos;

void main()
{
    vec2 dvx = vec2(dFdx(ioPlanePos.x), dFdy(ioPlanePos.x));
    vec2 dvy = vec2(dFdx(ioPlanePos.y), dFdy(ioPlanePos.y));

    float lx = length(dvx);
    float ly = length(dvy);

    vec2 dudv = vec2(lx, ly);
    float ldudv = length(dudv);

    float LOD = max(0.0, (log(ldudv * uMinimalDistanceBetweenPixels / uGridCellSize) / log(10.0)) + 1.0);

    float gridCellSizeLod0 = uGridCellSize * pow(10.0, floor(LOD));
    float gridCellSizeLod1 = gridCellSizeLod0 * 10.0;
    float gridCellSizeLod2 = gridCellSizeLod1 * 10.0;

    vec2 res = vec2(1.0) - (mod(ioPlanePos, gridCellSizeLod0) / (dudv * 1.25));
    res = vec2(1.0) - abs(clamp(res, vec2(0.0), vec2(1.0)) * 2.0 - vec2(1.0));
    float lod0a = max(res.x, res.y);

    res = vec2(1.0) - (mod(ioPlanePos, gridCellSizeLod1) / (dudv * 2.4));
    res = vec2(1.0) - abs(clamp(res, vec2(0.0), vec2(1.0)) * 2.0 - vec2(1.0));
    float lod1a = max(res.x, res.y);

    res = vec2(1.0) - (mod(ioPlanePos, gridCellSizeLod2) / (dudv * 4.0));
    res = vec2(1.0) - abs(clamp(res, vec2(0.0), vec2(1.0)) * 2.0 - vec2(1.0));
    float lod2a = max(res.x, res.y);

    float lodFract = fract(LOD);

    vec4 color;
    if (lod2a > 0.0)
    {
        color = uGridColorThick;
        color.a *= lod2a;
    }
    else if (lod1a > 0.0)
    {
        color = mix(uGridColorThick, uGridColorThin, lodFract);
        color.a *= lod1a;
    }
    else
    {
        color = uGridColorThin;
        color.a *= lod0a * (1.0 - lodFract);
    }

    float fallOff = 1.0 - clamp(length(ioPlanePos) / uGlobalGridSize, 0.0, 1.0);
    color.a *= fallOff;

    // NEW: axis highlighting, drawn on top of the grid color
    // Distance in pixels from each axis line (planePos.y == 0 is the "X axis", planePos.x == 0 is the "Y axis")
    vec2 axisDist = abs(ioPlanePos) / dudv; // in pixel units
    float axisXLine = 1.0 - clamp(axisDist.y / uAxisLineWidth, 0.0, 1.0); // line along Y=0 -> the X axis
    float axisYLine = 1.0 - clamp(axisDist.x / uAxisLineWidth, 0.0, 1.0); // line along X=0 -> the Y axis

    vec4 axisColor = vec4(0.0);
    float axisAlpha = 0.0;

    if (axisXLine > axisYLine)
    {
        axisColor = uAxisColorX;
        axisAlpha = axisXLine;
    }
    else if (axisYLine > 0.0)
    {
        axisColor = uAxisColorY;
        axisAlpha = axisYLine;
    }

    axisAlpha *= fallOff;
    color.rgb = mix(color.rgb, axisColor.rgb, axisAlpha);
    color.a = max(color.a, axisAlpha * axisColor.a);

    if (color.a < 0.01)
    discard;

    FragColor = color;
}