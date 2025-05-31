R""(

const float S_distanceEpsilon = 1e-2;
const float S_normalEpsilon = 1e-3;
const int S_maxSteps = 200;
const float S_maxDistance = 500.0;
const float S_distanceMultiplier = 0.7;
const float S_minVolumetricJumpDistance = 0.02;
const float S_volumetricDistanceMultiplier = 0.75;
const int S_reflectionJumps = 2;

#define S_VOLUMETRIC 0
#define S_REFLECTIONS 1
#define S_REFRACTIONS 0

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
const int tunnelType = 2;
const int waterType = 4;

vec3 getAmbientColor(int type)
{
    switch (type) {
        case sphereType:
            return vec3(1, 0, 0);
        case tunnelType:
            return vec3(0.2, 0.2, 0.2);
        case waterType:
            return 0.1*vec3(1, 1, 1);
        default:
           return vec3(1, 0, 1);
    }
}

vec3 getColor(in MarchResult result)
{
    vec3 lightPosition = vec3(0, 0, 0);
    vec3 ambientColor = getAmbientColor(result.type);
    vec3 normal = normal(result.position);
    vec3 invLight = normalize(lightPosition - result.position);
    float diffuse = max(0., dot(invLight, normal));
    return ambientColor * (0.02 + 0.98*diffuse);
}

float getReflectiveIndex(int type) {
    switch (type) {
        case sphereType:
            return 0.0;
        case tunnelType:
            return 0.3;
        case waterType:
            return 0.5;
        default:
           return 0.0;
    }
}

float tunnel(in vec3 p)
{
    float d = -sdSphere(vec3(p.xy, 0), 4.0 + 0.5*sin(0.05*p.z));
    return d;
}

float water(in vec3 p)
{
    float d = sdPlane(p, vec4(0, 1, 0, 2));
    return d;
}

DistanceInfo map(in vec3 p)
{
   DistanceInfo box = {tunnel(p), tunnelType};
   DistanceInfo sphere = {sdSphere(p - vec3(cos(iTime), sin(iTime), -10), 1.0), sphereType};
   DistanceInfo waterDistance = {water(p), waterType}; 
   return un(waterDistance, un(box, sphere));
}

void main()
{
    float u = (fragCoord.x - 0.5);
    float v = (fragCoord.y - 0.5) * iResolution.y / iResolution.x;
    vec3 rayOrigin = (iCameraMatrix * vec4(u, v, -0.7, 1.0)).xyz;
    vec3 eye = (iCameraMatrix * vec4(0.0, 0.0, 0.0, 1)).xyz;
    vec3 rayDirection = normalize(rayOrigin - eye);

    vec3 color = march(rayOrigin, rayDirection);
    color /= (color + vec3(1.0));

    fragColor = vec4(pow(color, vec3(0.5)), 1.0);
}

)""
