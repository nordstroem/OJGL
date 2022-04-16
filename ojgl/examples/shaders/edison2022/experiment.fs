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

DistanceInfo map(in vec3 p)
{
    vec3 orgPos = p;
    p.z += 2.0;
    float s = 0.6;
    float px = floor((p.y-s/2)/s);
    pMod1(p.y, s);
    
    float a = pModPolar(p.xz, 6);
    p.x-=2.5;
    p.x -= 0.6*sin(0.3*px + 3*iTime + 2*a);

    float d2 = sdBox(orgPos - vec3(0, -3.0, 0.0), vec3(5, 8, 5));
    float d = sdBoxFrame(p.xyz, vec3(0.3, s/2-0.01, 0.3), 0.06);
    d = max(d2, d);
    DistanceInfo legs = {0.5*d, sphereType};

    p = orgPos;
    p.x -= 0.3*sin(0.3*p.y + 3*iTime);
    p.z += 2.0;
    p.y -= 4.1;
    
    d = sdCutHollowSphere(vec3(p.x, -p.y, p.z), 5, -1, 0.1);
    DistanceInfo head = {d, sphereType};
    p = orgPos;
    p.z += 2.0;
    
    d = sdPlane(p, vec4(0, 1, 0, 13));
    DistanceInfo floor = {d, wallType};
    DistanceInfo blob = un(head, legs);
    return un(floor, blob);
}

float getReflectiveIndex(int type)
{
    return type == wallType ? 0.5 : 0.0;
}

vec3 eye = vec3(0);

vec3 getColor(in MarchResult result)
{
    vec3 lightPosition = vec3(-5, 9, 7);
    if (result.type != invalidType) {
        vec3 ambient = 2.0*vec3(0.5, 0.5, 1.0);
        if (result.type == wallType) {
            ambient = 1.3*vec3(0.1, 0.1, 0.3);
        }
        
        vec3 invLight = normalize(lightPosition - result.position);
        vec3 normal = normal(result.position);
        float k = max(0.0, dot(result.rayDirection, reflect(invLight, normal)));
        float spec = 1 * pow(k, 50.0);

        float shadow = shadowFunction(result.position, lightPosition, 32);
        float diffuse = max(0., dot(invLight, normal)) * (0.1 + 0.9*shadow);

        return vec3(ambient * (0.06 + 0.96*diffuse) + 0.1*spec);
    } else {
        return vec3(0.0);
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

    fragColor = vec4(pow(color, vec3(1.0)), 1.0);

}

)""
