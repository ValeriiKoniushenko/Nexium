#version 460 core

uniform mat4 uProjAndView;
uniform vec3 uCameraPos;
uniform float uGlobalGridSize = 10000.0;

uniform vec3 uPlaneOrigin;
uniform vec3 uPlaneRight;
uniform vec3 uPlaneUp;
// uniform vec3 uPlaneNormal;

const vec2 Pos[4] = vec2[4] (
        vec2(-1.0, -1.0), // BL
        vec2(1.0, -1.0), // BR
        vec2(1.0, 1.0), // TR
        vec2(-1.0, 1.0)  // TL
);

const int Indicies[6] = int[6](0,2,1,2,0,3);

out vec3 ioWorldPos;
out vec2 ioPlanePos;   // 2D coords in the plane's own basis
out vec3 ioPlaneOrigin;

void main()
{
    int index = Indicies[gl_VertexID];
    vec2 local = Pos[index] * uGlobalGridSize;

    // Project camera position onto the plane (in plane-local 2D coords),
    // so the grid quad follows the camera like your original XZ version did.
    vec3 camRel = uCameraPos - uPlaneOrigin;
    vec2 camPlanePos = vec2(dot(camRel, uPlaneRight), dot(camRel, uPlaneUp));

    vec2 planePos = local + camPlanePos;

    // Reconstruct world position from plane-local 2D coords
    vec3 worldPos = uPlaneOrigin + uPlaneRight * planePos.x + uPlaneUp * planePos.y;

    ioWorldPos = worldPos;
    ioPlanePos = planePos;
    ioPlaneOrigin = uPlaneOrigin;

    gl_Position = uProjAndView * vec4(worldPos, 1.0);
}