R""(


const float S_distanceEpsilon = 1e-1;
const float S_normalEpsilon = 1e-3;
const int S_maxSteps = 100;
const float S_maxDistance = 250.0;
const float S_distanceMultiplier = 0.7;
const float S_minVolumetricJumpDistance = 0.02;
const float S_volumetricDistanceMultiplier = 0.75;
const int S_reflectionJumps = 2;

#define S_VOLUMETRIC 0
#define S_REFLECTIONS 0
#define S_REFRACTIONS 0

#include "common/noise.fs"
#include "common/primitives.fs"
#include "common/raymarch_utils.fs"
#include "common/utils.fs"

in vec2 fragCoord;
out vec4 fragColor;

uniform float iTime;
uniform vec2 iResolution;

const int sphereType = 1;

vec3 getColor(in MarchResult result)
{
    vec3 lightPosition = vec3(100, 10, 50);
    if (result.type == sphereType) {
        vec3 normal = normal(result.position);
        vec3 invLight = normalize(lightPosition - result.position);
        float diffuse = max(0., dot(invLight, normal));
        return vec3(1.0, 0.0, 0.0) * (0.02 + 0.98*diffuse);
    }
    return vec3(0.0);
}

float getReflectiveIndex(int type) {
    return 0.0;
}

DistanceInfo map(in vec3 p)
{
   DistanceInfo d = {sdSphere(p, 1.0), sphereType};
   return d;
}


void main()
{
    float u = (fragCoord.x - 0.5);
    float v = (fragCoord.y - 0.5) * iResolution.y / iResolution.x;
    vec3 eye = vec3(0, 0, 10);
    vec3 rayOrigin = eye + vec3(u, v, -0.7);
    vec3 rayDirection = normalize(rayOrigin - eye);
    vec3 color = march(rayOrigin, rayDirection);
    fragColor = vec4(pow(color, vec3(0.5)), 1.0);

}

)""
