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

float grassDistance(vec3 p, float h, float w, float d) 
{
    float a = pModPolar(p.xz, 4);
    p.x-=d;
    p.x-= psin(iTime)*0.8*pow(0.15*(p.y+h), 2);
    float x = clamp((p.y + h)/(2*h), 0, 1);
    float r = w*max(0, pcurve(x, 0.2, 1.5));
    return sdBox(p.zyx, vec3(r, h, 0.2));
}

mat2 r05 = rot(0.5);

DistanceInfo map(in vec3 p, bool isMarch)
{
    vec3 orgP = p;
    p.y+=4;
    float d = grassDistance(p, 10, 2, 2);
    p.y -= 3.0;
    p.xz *= r05;
    float d2 = grassDistance(p, 10, 2, 1);
    p.y -= 3.0;
    p.xz *= r05;
    float d3 = grassDistance(p, 10, 1, 0.6);

    d = min(d3, min(d, d2));

    
    if (isMarch)
        at += 0.08/(1.2+d*d*d);
    
    DistanceInfo grass = {d, sphereType, 0.3*vec3(0.1, 0.3, 0.2)};


    p = orgP;
    d = sdPlane(p, vec4(0, 1, 0, 13));
    DistanceInfo floor = {d + 0.03*sin(0.5*p.x) + 0.03*cos(0.9*p.z), wallType, 0.5*vec3(0.0, 0.05, 0.2)};

    return un(floor, grass);
}

float getReflectiveIndex(int type)
{
    return type == wallType ? 0.3 : 0.2;
}

vec3 eye = vec3(0);

vec3 getColor(in MarchResult result)
{
    vec3 lightPosition = vec3(-8, 15, 20);
    if (result.type != invalidType) {
        vec3 ambient = result.color;
        
        vec3 invLight = normalize(lightPosition - result.position);
        vec3 normal = normal(result.position);
        float k = max(0.0, dot(result.rayDirection, reflect(invLight, normal)));
        float spec = 1 * pow(k, 50.0);

        float l = length(result.position.xz);
        float shadow = shadowFunction(result.position, lightPosition, 32);
        float diffuse = max(0., dot(invLight, normal));
        vec3 color = vec3(ambient * (0.1 + 0.96*diffuse));
        color += at * vec3(0.1, 0.3, 0.2);
        float fog = exp(-0.00035*l*l);

        color *= (0.2 + 0.8*shadow) * fog;
        return color;
    } else {
        vec3 color = vec3(0);
        //color += at * 1.2*vec3(0.1, 0.1, 0.3);
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
