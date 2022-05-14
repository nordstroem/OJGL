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
    vec3 orgP = p;


    float k = 0.3;
    p.x += 10*cos(k*p.y);
    p.z += 10*sin(k*p.y);
    float d = length(p.xz) - 5.5;
    float containerD = d;//= sdBoxFrame(p - vec3(0, dy, 0), vec3(10, 1+dy, 10), 1.0);
    
    p = orgP;
    float dy = 1.5*5;
    pMod3(p, vec3(3));
    d = sdBoxFrame(p, vec3(1), 1.0);
    d = max(containerD, d);
    DistanceInfo box = {d, sphereType, vec3(0.5, 0.5, 1.0)};
    if (isMarch)
        at += 0.5/(1.2+d*d*d);

    p = orgP;
    d = sdPlane(p, vec4(0, 1, 0, 13));
    float d2 = sdBox(p - vec3(0, -120, 0), vec3(100, 200, 100));
    DistanceInfo walls = {-d2, sphereType, vec3(0.1, 0.1, 0.5)};
    DistanceInfo floor = {d, wallType, vec3(0.1, 0.1, 0.5)};

    return un(floor, box);
}

float getReflectiveIndex(int type)
{
    return type == wallType ? 0.5 : 0.0;
}

vec3 eye = vec3(0);

vec3 getColor(in MarchResult result)
{
    vec3 lightPosition = vec3(-8, 35, 130);
    if (result.type != invalidType) {
        vec3 ambient = result.color;
        if (result.type == wallType) {
            ambient = 0.3*vec3(0.1, 0.1, 0.4);
        }
        
        vec3 invLight = normalize(lightPosition - result.position);
        vec3 normal = normal(result.position);
        float k = max(0.0, dot(result.rayDirection, reflect(invLight, normal)));
        float spec = 1 * pow(k, 50.0);

        float l = length(result.position.xz);
        float shadow = shadowFunction(result.position, lightPosition, 32);
        float diffuse = max(0., dot(invLight, normal));
        vec3 color = vec3(ambient * (0.1 + 0.96*diffuse) + 0.5*spec);
        color += at * 1.2*vec3(0.1, 0.1, 0.3);
        float fog = exp(-0.00035*l*l);

        color *= (0.2 + 0.8*shadow) * fog;
        return color;
    } else {
        vec3 color = vec3(0);
        color += at * 1.2*vec3(0.1, 0.1, 0.3);
        float l = length(result.position.xz);
        float fog = exp(-0.00035*l*l);

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
