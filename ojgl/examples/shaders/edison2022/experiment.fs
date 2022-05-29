R""(
#include "common/noise.fs"
#include "common/primitives.fs"
#include "edison2022/raymarch_settings.fs"
#include "common/raymarch_utils.fs"
#include "common/utils.fs"

in vec2 fragCoord;
out vec4 fragColor;

uniform float iTime;
uniform vec2 iResolution;
uniform mat4 iCameraMatrix;

const int sphereType = 1;
const int wallType = 2;

float at = 0.0;
float at2 = 0.0;

const float pi = 3.14159256;


DistanceInfo map(in vec3 p, bool isMarch)
{

    vec3 kr = pMod3(p.xyz, vec3(50,80,50));
    float t = mod(iTime + 6*psin(noise_3(kr)), 6);
    vec3 orgPos = p;
    p.x -= 10*noise_3(kr);
    p.z -= 10*noise_3(kr);
    p.y += 0.5*sin(iTime);
    p.y -= 2*t + 3*floor(t / 2) + 3*smoothstep(0, 1, mod(t, 2));
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
    vec3 legColor = 1.0*vec3(0.5, 0.5, 1.0);
    legColor *= 0.2 + 0.5*psin(px*5);
    DistanceInfo legs = {0.5*d, sphereType, legColor };

    if (isMarch)
        at += 0.008/(1.2+d*d*d);

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
    DistanceInfo head = {d, sphereType, headColor};
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

vec3 getColor(in MarchResult result)
{
    vec3 lightPosition = vec3(-35, 30, 38);
    float l = length(result.position);
    float ll = abs(result.position.y +5);
    float fog = exp(-0.00015*l*l);

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
        color += at * 1.2*vec3(0.1, 0.1, 0.3);

        color *= (0.2 + 0.8*shadow);
        return color;
    } else {
        vec3 color = 0.03*vec3(0.0, 0.02, 0.1);
        color += at * 1.2*vec3(0.1, 0.1, 0.3);
        return color;
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

    // Tone mapping
    color /= (color + vec3(1.0));

    fragColor = vec4(pow(color, vec3(0.5)), 1.0);

}

)""
