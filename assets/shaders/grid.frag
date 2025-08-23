#version 460 core

out vec4 FragColor;

uniform float uGlobalGridSize = 10000.0;
uniform float uMinimalDistanceBetweenPixels = 2.0;
uniform float uGridCellSize = 25.0;
uniform vec4 uGridColorThin = vec4(0.5, 0.5, 0.5, 1.0);
uniform vec4 uGridColorThick = vec4(0.0, 0.0, 0.0, 1.0);

in vec3 ioWorldPos;

void main()
{
    vec2 dvx = vec2(dFdx(ioWorldPos.x), dFdy(ioWorldPos.x));
    vec2 dvy = vec2(dFdx(ioWorldPos.z), dFdy(ioWorldPos.z));

    float lx = length(dvx);
    float ly = length(dvy);

    vec2 dudv = vec2(lx, ly);
    float ldudv = length(dudv);

    float LOD = max(0.0, (log(ldudv * uMinimalDistanceBetweenPixels / uGridCellSize) / log(10.0)) + 1.0);

    float gridCellSizeLod0 = uGridCellSize * pow(10.0, floor(LOD));
    float gridCellSizeLod1 = gridCellSizeLod0 * 10.0;
    float gridCellSizeLod2 = gridCellSizeLod1 * 10.0;

    dudv *= 2.0;

    vec2 res = vec2(1.0) - (mod(ioWorldPos.xz, gridCellSizeLod0) / dudv);
    res = vec2(1.0) - abs(clamp(res, vec2(0.0), vec2(1.0)) * 2.0 - vec2(1.0));
    float lod0a = max(res.x, res.y);

    res = vec2(1.0) - (mod(ioWorldPos.xz, gridCellSizeLod1) / dudv);
    res = vec2(1.0) - abs(clamp(res, vec2(0.0), vec2(1.0)) * 2.0 - vec2(1.0));
    float lod1a = max(res.x, res.y);

    res = vec2(1.0) - (mod(ioWorldPos.xz, gridCellSizeLod2) / dudv);
    res = vec2(1.0) - abs(clamp(res, vec2(0.0), vec2(1.0)) * 2.0 - vec2(1.0));
    float lod2a = max(res.x, res.y);

    float lodFract = fract(LOD);

    vec4 color;
    if (lod2a > 0.0)
    {
        color = uGridColorThick;
        color.a *= lod2a;
    }
    else
    {
        if (lod1a > 0.0)
        {
            color = mix(uGridColorThick, uGridColorThin, lodFract);
            color.a *= lod1a;
        }
        else
        {
            color = uGridColorThin;
            color.a *= lod0a * (1.0 - lodFract);
        }
    }

    float fallOff = 1.0 - clamp(length(ioWorldPos.xz) / uGlobalGridSize, 0.0, 1.0);
    color.a *= fallOff;

    FragColor = color;
}