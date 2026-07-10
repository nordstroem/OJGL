R""(
const float S_distanceEpsilon = 2e-3;
const float S_normalEpsilon = 1e-2;
const int S_maxSteps = 100;
const float S_maxDistance = 400.0;
const float S_distanceMultiplier = 1.0;
const float S_minVolumetricJumpDistance = 0.02;
const float S_volumetricDistanceMultiplier = 0.75;
const int S_reflectionJumps = 3;

#define S_VOLUMETRIC 0
#define S_REFLECTIONS 1

#include "common/primitives.fs"
#include "common/raymarch_utils.fs"
#include "common/utils.fs"

in vec2 fragCoord;
out vec4 fragColor;

uniform float iTime;
uniform vec2 iResolution;
uniform mat4 iCameraMatrix;
uniform float C_0_S;
uniform float C_0_T;

const int sphereType = 1;
const int roomType = 2;

DistanceInfo object(in vec3 p)
{
    p.z += 5.0;
    return DistanceInfo(sdSphere(p, 1.0), sphereType);
}

DistanceInfo room(in vec3 p)
{
    p.z += 5.0;
    return DistanceInfo(-sdBox(p, vec3(10.0, 10.0, 15.0)), roomType);
}

DistanceInfo map(in vec3 p)
{
    return un(object(p), room(p));
}

float getReflectiveIndex(int type)
{
    return 0.5;
}

vec3 getColor(in MarchResult result)
{
    if (result.type == invalidType) {
        return vec3(0.0);
    }

    vec3 lightPosition = vec3(3.0, 4.0, -3.0);
    vec3 normal = normal(result.position);
    vec3 invLight = normalize(lightPosition - result.position);
    float diffuse = max(0.0, dot(invLight, normal));

    if (result.type == sphereType) {
        float pulse = exp(-C_0_S * 6.0); // bright flash right after a Clinkster sync trigger, decaying over ~0.3-0.5s
        return vec3(0.2, 0.5, 0.9) * (0.1 + 0.9 * diffuse) * (1.0 + 2.0 * pulse);
    } else {
        return vec3(0.5, 0.5, 0.55) * (0.05 + 0.95 * diffuse);
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
