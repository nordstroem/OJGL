R""(
#version 430
#include "common/noise.fs"
#include "common/primitives.fs"
#include "edison2021/raymarch_settings.fs"
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
    p.xy *= rot(iTime);
    DistanceInfo box = { sdBox(p, vec3(1.5, 0.2, 2.0)), sphereType };
    p.x = abs(p.x);
    p.x -= 2;
    p.zy *= rot(iTime);
    DistanceInfo box2 = { sdBox(p, vec3(1.0, 0.2, 0.5)), sphereType };
    return un(box, box2);
}

float getReflectiveIndex(int type)
{
    return type == wallType ? 0.3 : 0.0;
}

/*
vec3 getAmbientColor(int type) {
}
*/
vec3 getColor(in MarchResult result)
{
    vec3 lightPosition = vec3(10, 10, 0);
    if (result.type != invalidType) {
        vec3 ambient = vec3(0.8, 0.8, 0.1);
        vec3 invLight = normalize(lightPosition - result.position);
        vec3 normal = normal(result.position);
        float shadow = shadowFunction(result.position, lightPosition, 32);
        float diffuse = max(0., dot(invLight, normal)) * (1);
        return vec3(ambient * (0.04 + 0.96*diffuse));
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

    vec3 color = march(rayOrigin, rayDirection);

    // Tone mapping
    color /= (color + vec3(1.0));

    fragColor = vec4(pow(color, vec3(0.4545)), 1.0);

}

)""
