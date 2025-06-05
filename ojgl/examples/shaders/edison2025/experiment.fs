R""(

const float S_distanceEpsilon = 1e-3;
const float S_normalEpsilon = 5e-2;
const int S_maxSteps = 600;
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
uniform sampler2D inTexture0;

const int sphereType = 1;
const int mountainType = 2;
const int waterType = 4;

vec3 cameraPosition;
vec3 rayDirection;

vec3 getAmbientColor(int type, vec3 pos, vec3 normal)
{
    switch (type) {
        case sphereType:
            return vec3(1, 0, 0);
        case mountainType: 
            return 0.0*vec3(0.2, 0.2, 0.1);
        case waterType:
            return vec3(0.1, 0.1, 0.7);
        default:
           return vec3(1, 0, 1);
    }
}

vec3 getColor(in MarchResult result)
{
    vec3 lightPosition = vec3(-100, 20, 200);
    vec3 normal = normal(result.position);
    vec3 invLight = normalize(lightPosition - result.position);
    float diffuse = max(0., dot(invLight, normal));
    vec3 ambientColor = getAmbientColor(result.type, result.position, normal);
    vec3 color = ambientColor * (0.02 + 0.98*diffuse);
    vec3 ao = vec3(float(result.steps) / 600);
    float k = max(0.0, dot(rayDirection, reflect(invLight, normal)));
    float spec = 1 * pow(k, 30.0);
    color += spec;

    return mix(color, ao, 0.75);
}

float getReflectiveIndex(int type) {
    switch (type) {
        case sphereType:
            return 0.0;
        case mountainType:
            return 0.0;
        case waterType:
            return 1.0;
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
    float d = sdPlane(p, vec4(0, 1, 0, -10)) + 0.002*noise_2(5*p.xz + iTime);
    return d;
}

float mountain(vec3 p) {
    const float r = max(0, length(p.xz) - 60);
    const float k = 40 * exp(-0.006*r);
    if (p.y > k) {
        return sdPlane(p, vec4(0, 1, 0, k));
    }
	float h = 4*texture(inTexture0, (p.xz)/90.0).x + 
              200*pow(texture(inTexture0, (p.xz)/1600.0).x, 4);

	return p.y - h + 0.5;
}

DistanceInfo map(in vec3 p)
{
   DistanceInfo box = {mountain(p), mountainType};
   DistanceInfo sphere = {sdSphere(p - vec3(cos(iTime), 15 + sin(iTime), 0), 1.0), sphereType};
   DistanceInfo waterInfo = {water(p), waterType};
   return un(waterInfo, un(box, sphere));
}

void main()
{
    float u = (fragCoord.x - 0.5);
    float v = (fragCoord.y - 0.5) * iResolution.y / iResolution.x;
    vec3 rayOrigin = (iCameraMatrix * vec4(u, v, -0.7, 1.0)).xyz;
    cameraPosition = (iCameraMatrix * vec4(0.0, 0.0, 0.0, 1)).xyz;
    rayDirection = normalize(rayOrigin - cameraPosition);

    vec3 color = march(rayOrigin, rayDirection);
    // color /= (color + vec3(1.0));

    fragColor = vec4(pow(color, vec3(0.5)), 1.0);
}

)""
