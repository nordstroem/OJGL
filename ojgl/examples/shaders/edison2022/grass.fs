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

float pcurve( float x, float a, float b )
{
    const float k = pow(a+b,a+b)/(pow(a,a)*pow(b,b));
    return k*pow(x,a)*pow(1.0-x,b);
}

DistanceInfo map(in vec3 p, bool isMarch)
{
    vec3 orgP = p;
    pModPolar(p.xz, 10);
    p.x-=6;
    p.x-= pow(0.15*(p.y+10), 2);
    float x = clamp((p.y + 10)/20, 0, 1);
    float r = 2*max(0, pcurve(x, 0.2, 1.5));
    float d = sdBox(p.zyx, vec3(r, 10, 0.2));
    DistanceInfo grass = {d, sphereType, vec3(1.0)};

    p = orgP;
    d = sdPlane(p, vec4(0, 1, 0, 13));
    DistanceInfo floor = {d, wallType, vec3(0.1, 0.1, 0.5)};

    return un(floor, grass);
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
