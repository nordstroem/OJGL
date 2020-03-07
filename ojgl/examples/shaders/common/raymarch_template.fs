R""(
#version 430
#include "common/noise.fs"
#include "common/primitives.fs"
#include "common/raymarch_utils.fs"
#include "common/utils.fs"

in vec2 fragCoord;
out vec4 fragColor;

uniform float iTime;
uniform vec2 iResolution;
uniform mat4 iCameraMatrix;

const int sphereType = 1;
const int wallType = 2;

DistanceInfo map(in vec3 p)
{
    DistanceInfo walls = { -sdBox(p, vec3(2.0, 2.0, 4.0)), wallType };
    DistanceInfo sphere2 = { sdSphere(p - vec3(1.0, 1.0, -1.0), 0.1), sphereType };
    pModPolar(p.xy, 5);
    p.x -= 0.6;
    DistanceInfo sphere1 = { sdSphere(p, 0.25) + 0.05 * noise_3(p.xyz * 10.0 + iTime), sphereType };
    return un(un(sphere1, sphere2), walls);
}

float getReflectiveIndex(int type)
{
    return type == wallType ? 0.6 : 0.0;
}

vec3 getColor(in MarchResult result)
{
    if (result.type != invalidType) {
        vec3 ambient = vec3(0.1, 0.1, 0.1 + 0.5 * sin(result.type + 1));
        vec3 invLight = -normalize(vec3(-0.7, -0.2, -0.5));
        vec3 normal = normal(result.position);
        float diffuse = max(0., dot(invLight, normal));
        return vec3(ambient * (1.0 + diffuse));
    } else {
        return vec3(0.0);
    }
}

void main()
{
    float u = (fragCoord.x - 0.5);
    float v = (fragCoord.y - 0.5) * iResolution.y / iResolution.x;
    vec3 rayOrigin = (iCameraMatrix * vec4(u, v, -1.0, 1.0)).xyz;
    vec3 eye = (iCameraMatrix * vec4(0.0, 0.0, 0.0, 1.0)).xyz;
    vec3 rayDirection = normalize(rayOrigin - eye);

    MarchResult result = march(rayOrigin, rayDirection);
    vec3 color = getColor(result);

    float reflectiveIndex = getReflectiveIndex(result.type);
    if (reflectiveIndex > 0.0) {
        rayDirection = reflect(rayDirection, normal(result.position));
        result = march(result.position + 0.1 * rayDirection, rayDirection);
        vec3 newColor = getColor(result);
        color = mix(color, newColor, reflectiveIndex);
    }

    fragColor = vec4(color, 1.0);
}

)""
