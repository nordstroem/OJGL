R""(
const float S_distanceEpsilon = 1e-2;
const float S_normalEpsilon = 1e-2;
const int S_maxSteps = 100;
const float S_maxDistance = 400.0;
const float S_distanceMultiplier = 1.0;
const float S_minVolumetricJumpDistance = 0.02;
const float S_volumetricDistanceMultiplier = 0.75;
const int S_reflectionJumps = 2;

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

vec3 gEye;
float gFresnel = 0.0;
float gFocusDistance = 0.0;
const vec3 S_focusTarget = vec3(0.0, 0.0, 0.0);
const float S_focusStrength = 0.03;
vec3 cRoomSize = vec3(20, 20, 20);

float GridPattern(in vec2 uv)
{
  return 0.5*clamp(10.*sin(PI*uv.x) + 10.5, 0.0, 1.0)
       / 0.5*clamp(10.*sin(PI*uv.y) + 10.5, 0.0, 1.0);
}

float SquareHolePattern(in vec2 uv)
{
  float thickness = 4.0;
  float t = cos(uv.x*2.0) * cos(uv.y*2.0) / thickness;
  return smoothstep(0.1, 0.0, t*t);
}

DistanceInfo scene1(in vec3 p)
{
    pMod1(p.x, 4.0);
    pMod1(p.z, 15.0);
    pMod1(p.y, 5.0);
    float r = 0.5;
    p.z -= r*cos(5*iTime);
    p.x -= r*sin(3*iTime);
    p.y -= r*sin(3*iTime);
    float d1 = sdSphere(p, 1.0);
    float d2 = sdBox(p, vec3(1.0));
    float d = mix(d1, d2, 0.5 * (1+  sin(iTime)));
    return DistanceInfo(d, sphereType);
}

float wallPattern(in vec2 uv) {
    float thickness = 2.0;
    float t = cos(uv.x*2.0) * cos(uv.y*2.0) / thickness;
    return smoothstep(0.1, 0.0, t*t);
}

DistanceInfo room(in vec3 p)
{
    p.y -= wallPattern(p.xz) * 0.005;
    p.x -= wallPattern(p.zy) * 0.005;
    p.z -= wallPattern(p.xy) * 0.005;
    return DistanceInfo(-sdBox(p, cRoomSize), roomType);
}

DistanceInfo map(in vec3 p)
{
    return un(scene1(p), room(p));
}

const float roomEdgeBevel = 2.7;
float roomEdgeAmount(in vec3 p)
{
    vec3 q = p;
    vec3 d = abs(q) - cRoomSize;
    vec3 w = smoothstep(-roomEdgeBevel, 0.0, d);
    return clamp(w.x + w.y + w.z - 1.0, 0.0, 1.0);
}

float getReflectiveIndex(int type)
{
    if (type == sphereType){
        return mix(0.3, 0.9, gFresnel);
    }
    return 0.0;
}

vec3 getColor(in MarchResult result)
{
    if (result.jump == 0) {
        gFocusDistance = length(gEye - result.position);
    }

    if (result.type == invalidType) {
        return vec3(0.0);
    }

    vec3 lightPosition = vec3(3.0, 4.0, 3.0);
    vec3 normal = normal(result.position);
    vec3 invLight = normalize(lightPosition - result.position);
    vec3 viewDir = normalize(gEye - result.position);
    float diffuse = max(0.0, dot(invLight, normal));
    vec3 halfDir = normalize(invLight + viewDir);
    float specular = pow(max(0.0, dot(normal, halfDir)), 32.0);

    if (result.type == sphereType) {
        float pulse = exp(-C_0_S * 6.0);
        vec3 metalColor = 0.5*vec3(0.2, 0.5, 0.9);
        gFresnel = pow(1.0 - max(0.0, dot(normal, viewDir)), 4.0);
        vec3 baseColor = metalColor * diffuse * (1.0 + 2.0 * pulse);
        vec3 tintedSpecular = specular * mix(vec3(1.0), metalColor, 0.6); 
        return baseColor + 2.0 * gFresnel * mix(vec3(0.6, 0.8, 1.0), metalColor, 0.4) + tintedSpecular;
    } else {
        float pulse = exp(-C_0_S * 6.0);
        float edgeAmount = roomEdgeAmount(result.position);
        vec3 metalColor = 0.03*vec3(0.2, 0.3, 0.3);
        gFresnel = 0.1*pow(1.0 - max(0.0, dot(normal, viewDir)), 4.0);
        vec3 baseColor = metalColor * diffuse * (1.0 + 2.0 * pulse);
        vec3 tintedSpecular = specular * mix(vec3(1.0), metalColor, 0.6);
        vec3 col = baseColor + 2.0 * gFresnel * mix(vec3(0.6, 0.8, 1.0), metalColor, 0.4) + tintedSpecular;
        // float shadow = 0.3 + 0.7*shadowFunction(result.position, lightPosition, 30);
        return col * (1.0 - 0.4*edgeAmount);

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
    gEye = (iCameraMatrix * vec4(0.0, 0.0, 0.0, 1.0)).xyz;
    vec3 rayDirection = normalize(rayOrigin - gEye);

    vec3 color = march(rayOrigin, rayDirection);

    float focalDistance = length(gEye - S_focusTarget);
    float focus = clamp( (abs(gFocusDistance - focalDistance)-7.5) * S_focusStrength, 0.0, 1.0);

    fragColor = vec4(pow(max(color, 0.0), vec3(0.4545)), focus);
}
)""
