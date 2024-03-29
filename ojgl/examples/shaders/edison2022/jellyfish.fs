R""(
const float S_distanceEpsilon = 1e-3;
const float S_normalEpsilon = 1e-3;
const int S_maxSteps = 230;
const float S_maxDistance = 220.0;
const float S_distanceMultiplier = 0.7;
const float S_minVolumetricJumpDistance = 0.02;
const float S_volumetricDistanceMultiplier = 0.75;
const int S_reflectionJumps = 2;
#define S_VOLUMETRIC 0
#define S_REFLECTIONS 0
#define S_REFRACTIONS 0

#include "common/noise.fs"
#include "common/primitives.fs"
#include "edison2022/raymarch_utils.fs"
#include "common/utils.fs"

in vec2 fragCoord;
out vec4 fragColor;

uniform float iTime;
uniform vec2 iResolution;
uniform mat4 iCameraMatrix;
uniform float C_2_S;
uniform float C_2_T;

const int sphereType = 1;
const int wallType = 2;

float at = 0.0;
float at2 = 0.0;

const float pi = 3.14159256;

bool isSecondScene() {
    return iTime > 11;
}
DistanceInfo map(in vec3 p, bool isMarch, vec3 dummy)
{
    bool isSecond = isSecondScene();
    p.y-= isSecond ? 1.5 * (iTime - 12) : 3.2 * (iTime +1);
    p.y += 0.5*sin(iTime);
    vec3 absOrgPos = p;
    float k = isSecond ? 5 : 1;
    vec3 bBoxSize = vec3(25+k*50, 30 + (k-1)*80, 25 + k*50);
    float bBoxD = sdBox(absOrgPos + vec3(0, 0, 0), bBoxSize);
    vec3 kr = pMod3(p.xyz, vec3(50,80,50));
    float o = 2*noise_3(7*kr);
    float t = mod(iTime, 28);
    t += o;
    vec3 orgPos = p;
    p.x -= 10*noise_3(50*kr+6) - 2;
    p.z -= 10*noise_3(50*kr+5) - 7;
    p.y += 30*noise_3(50*kr - 10) +5;

    if (kr.z == 2 && kr.y == 0 && kr.x == 0) {
        p.y += 30;


    }
    p.y -= 0.5 * (0 + 3*floor(t / 2) + 3*smoothstep(0, 1, mod(t, 2)));
    p.xz *= rot(0.2*sin(iTime));
    vec3 orgOctoP = p;

    float s = 0.6;
    float px = floor((p.y-s/2)/s);
    pMod1(p.y, s);

    float a = pModPolar(p.xz, 6);
    p.x -= 2.5;

    float ep = 0.12*px + 4*(mod(t-1.2, 2) - 1);
    p.x -= 0.3*sin(0.3*px + 3*iTime + 2*a) + 2*exp(-ep*ep);


    float d2 = sdBox(orgOctoP - vec3(0, -3.0, 0.0), vec3(8, 8, 8));
    float r = 0.2;
    float d = sdBox(p.xyz, vec3(r, s/2-0.01, r));
    d = max(d2, d);
    d = max(bBoxD, d);
    vec3 legColor = 1.0*vec3(0.5, 0.5, 1.0);
    legColor *= 0.2 + 0.5*psin(px*5);
    DistanceInfo legs = {0.5*d, sphereType, legColor };

    if (isMarch)
        at += 0.006/(1.2+d*d*d);

    p = orgOctoP;
    p.x -= 0.3*sin(0.3*p.y + 3*iTime);
    p.y -= 4.1;

    vec3 headColor = 20.0*vec3(0.2, 0.3, 1.0);
    headColor.x *= 0.2 + 0.8*psin(10*kr.x + kr.z);
    headColor.y *= 0.2 + 0.8*psin(3*kr.z + 10);
    headColor.z *= 0.2 + 0.8*psin(2*kr.y + 20);

    ep = 4*(mod(t-1.2, 2)-1);
    ep = exp(-ep*ep);
    d = sdCutHollowSphere(vec3(p.x, -p.y-3.75*ep, p.z), 5+2*ep, -1-3*ep, 0.1);

    d = max(bBoxD, d);
    DistanceInfo head = {d, sphereType, 2*headColor};
    if (isMarch)
        at += 0.1/(1.2+d*d*d);

    p = orgPos;


    d = sdPlane(p, vec4(0, 1, 0, 13)) + 0.05*noise_2(p.xz + vec2(iTime, iTime*0.2));
    DistanceInfo floor = {d, wallType, 0.3*vec3(0.0, 0.02, 0.1)};
    DistanceInfo blob = un(head, legs);
    p = orgPos;

    return blob;
}

float getReflectiveIndex(int type)
{
    return type == wallType ? 0.0 : 0.0;
}

vec3 eye = vec3(0);
vec3 marchPosition = vec3(0);

vec3 getColor(in MarchResult result)
{
    vec3 lightPosition = vec3(-35, 30, 238);
    float l = length(result.position - eye);
    float ll = abs(result.position.y +5);
    float fog = exp(-0.00015*l*l);
    marchPosition = result.position;

    vec3 lightColor = vec3(0.1, 0.3, 1.3) * 1 / (0.2+ pow(C_2_S, 0.7));
    if (result.type != invalidType) {
        vec3 ambient = result.color;

        vec3 invLight = normalize(lightPosition - result.position);
        vec3 normal = normal(result.position);
        float k = max(0.0, dot(result.rayDirection, reflect(invLight, normal)));
        float spec = 1 * pow(k, 50.0);


        float shadow = 1.0;
        if (result.type == wallType)
            shadow = shadowFunction(result.position, lightPosition, 32);
        float diffuse = max(0., dot(invLight, normal));
        vec3 color = vec3(ambient * (0.01 + 0.99*diffuse));
        color += at * 15.2*lightColor;

        color *= (0.2 + 0.8*shadow);
        return color  * fog;
    } else {
        vec3 color = 0.03*vec3(0.0, 0.02, 0.1);
        color += at * 105.2*lightColor;
        return color  * fog;
    }
}


void main()
{
    float u = (fragCoord.x - 0.5);
    float v = (fragCoord.y - 0.5) * iResolution.y / iResolution.x;
    vec3 rayOrigin = (iCameraMatrix * vec4(u, v, -0.7, 1.0)).xyz;
    eye = (iCameraMatrix * vec4(0.0, 0.0, 0.0, 1)).xyz;
    vec3 rayDirection = normalize(rayOrigin - eye);

    vec3 color = march(rayOrigin, rayDirection);



    color /= (color + vec3(1.0));

    float focus = 0.0;

    if (isSecondScene()) {
        const vec3 focusPos = eye + vec3(-30, -20, 20);
        float l = length(firstJumpPosition - focusPos) - 20;
        focus = min(1.0, abs( max(0, l) * 0.0025));
    }
    fragColor = vec4(pow(color, vec3(0.5)), focus);

}

)""