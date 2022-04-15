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
    vec3 orgP = p;
    p.xz = rot(0.1*iTime) * p.xz;
    pModPolar(p.xy, 15);
    p.x-=0.7;
    pModPolar(p.xz, 45);
    p.x-=0.6;
    pModPolar(p.zy, 4);
    p.z-=-0.06;
    mo(p.xz, vec2(2.0, 0.1));
    float d = sdBoxFrame(p, vec3(0.1), 0.003);
    DistanceInfo sphere = {d, sphereType };

    p = orgP;

    d = -sdBox(p, vec3(30));
    DistanceInfo worldBox = {d, wallType};
    return un(sphere, worldBox);
}

float getReflectiveIndex(int type)
{
    return type == wallType ? 0.1 : 0.0;
}

vec3 getColor(in MarchResult result)
{
    vec3 lightPosition = vec3(0, 0, 5);
    if (result.type != invalidType) {
        vec3 ambient = vec3(0.6, 0.02, 0.02);
        if (result.type == wallType) {
            ambient = vec3(0.2);
        }
        vec3 invLight = normalize(lightPosition - result.position);
        vec3 normal = normal(result.position);
        
        float shadow = shadowFunction(result.position, lightPosition, 32);
        float diffuse = max(0., dot(invLight, normal)) * (shadow);
        return vec3(ambient * (0.04 + 0.96*diffuse));
    } else {
        return vec3(0.0);
    }
}


void main()
{
    float u = (fragCoord.x - 0.5);
    float v = (fragCoord.y - 0.5) * iResolution.y / iResolution.x;
    vec3 rayOrigin = (iCameraMatrix * vec4(u, v, -1.0, 1.0)).xyz;
    vec3 eye = (iCameraMatrix * vec4(0.0, 0.0, 0.0, 1.0)).xyz;
    vec3 rayDirection = normalize(rayOrigin - eye);

    vec3 color = march(rayOrigin, rayDirection);

    // Tone mapping
    color /= (color + vec3(1.0));

    fragColor = vec4(pow(color, vec3(0.4545)), 1.0);

}

)""
