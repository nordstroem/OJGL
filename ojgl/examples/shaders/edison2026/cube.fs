R""(
#include "common/primitives.fs"
#include "common/raymarch_settings.fs"
#include "common/raymarch_utils.fs"
#include "common/utils.fs"

in vec2 fragCoord;
out vec4 fragColor;

uniform float iTime;
uniform vec2 iResolution;
uniform mat4 iCameraMatrix;

const int cubeType = 1;

DistanceInfo map(in vec3 p)
{
    p.z += 5.0;
    p.xy *= rot(iTime);
    p.yz *= rot(iTime * 0.5);
    return DistanceInfo(sdBox(p, vec3(1.0)), cubeType);
}

float getReflectiveIndex(int type)
{
    return 0.0;
}

vec3 getColor(in MarchResult result)
{
    if (result.type != invalidType) {
        vec3 lightPosition = vec3(3.0, 4.0, -3.0);
        vec3 normal = normal(result.position);
        vec3 invLight = normalize(lightPosition - result.position);
        float diffuse = max(0.0, dot(invLight, normal));
        return vec3(0.2, 0.5, 0.9) * (0.1 + 0.9 * diffuse);
    } else {
        return vec3(0.0);
    }
}

VolumetricResult evaluateLight(in vec3 p)
{
    return VolumetricResult(1e6, vec3(0.0));
}

float getFogAmount(in vec3 p)
{
    return 0.0;
}

void main()
{
    float u = (fragCoord.x - 0.5);
    float v = (fragCoord.y - 0.5) * iResolution.y / iResolution.x;
    vec3 rayOrigin = (iCameraMatrix * vec4(u, v, -1.0, 1.0)).xyz;
    vec3 eye = (iCameraMatrix * vec4(0.0, 0.0, 0.0, 1.0)).xyz;
    vec3 rayDirection = normalize(rayOrigin - eye);

    vec3 color = march(rayOrigin, rayDirection);

    fragColor = vec4(pow(color, vec3(0.4545)), 1.0);
}

)""
