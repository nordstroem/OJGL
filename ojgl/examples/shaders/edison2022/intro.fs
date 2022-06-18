R""(
#include "common/noise.fs"
#include "common/primitives.fs"
#include "edison2022/raymarch_settings.fs"
#include "common/raymarch_utils.fs"
#include "common/utils.fs"

in vec2 fragCoord;
out vec4 fragColor;

uniform sampler2D ojText;
uniform float iTime;
uniform vec2 iResolution;
uniform mat4 iCameraMatrix;

const int sphereType = 1;
const int wallType = 2;

float at = 0.0;
float at2 = 0.0;

vec3 fogColor = vec3(0.0);

const float pi = 3.14159256;

float pcurve( float x, float a, float b )
{
    const float k = pow(a+b,a+b)/(pow(a,a)*pow(b,b));
    return k*pow(x,a)*pow(1.0-x,b);
}

mat2 r05 = rot(0.5);


vec3 jellyfishPosition() {
    vec3 p = vec3(0.f);
    float t = mod(iTime, 15);
    p.y+=20;
    p.z-=30;
    p.x-=-5;
    vec3 orgPos = p;
    p.y += 0.5*sin(iTime);
    p.y -= 0.5 * (2*t + 3*floor(t / 2) + 3*smoothstep(0, 1, mod(t, 2)));
    return p;
}

DistanceInfo jellyfish(in vec3 p, bool isMarch)
{

    p.xz *= rot(-0.2);
    p = vec3(p.y, -p.x, p.z);//
    float t = mod(iTime, 15);
    p += jellyfishPosition();
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
    //if (isMarch)
    //    at += 0.0008/(1.2+d*d*d);

    p = orgOctoP;
    p.x -= 0.3*sin(0.3*p.y + 3*iTime);
    p.y -= 4.1;
    
    vec3 headColor = 20.0*vec3(0.2, 0.3, 1.0);

    ep = 4*(mod(t-1.2, 2)-1);
    ep = exp(-ep*ep);
    d = sdCutHollowSphere(vec3(p.x, -p.y-3.75*ep, p.z), 5+2*ep, -1-3*ep, 0.1);
    DistanceInfo head = {d, sphereType, headColor};
    //if (isMarch)
    //    at += 0.01/(1.2+d*d*d);

    DistanceInfo blob = un(head, legs);

    return blob;

}
DistanceInfo map(in vec3 p, bool isMarch)
{
    vec3 orgP = p;
    p = orgP;
    vec2 ojUV = clamp(vec2(-p.z / 20 + 1, -p.x / 20 + 1), vec2(0.0), vec2(1.0));
    bool onOJ = texture(ojText, ojUV).x < 0.5;
    vec3 floorColor = 0.67*vec3(0.0, 0.02, 0.05);

    if (onOJ) {
        floorColor = 0.05*vec3(1.0);
       //floorColor *= 0.5;
       p.y-=0.5 +  0.1*noise_2(3*p.xz);
       //float d = sdPlane(p, vec4(0, 1, 0, 13));
     //  floorColor += 0.01/(0.05+0.9*d*d*d) * vec3(3.0, 3.0, 0.4);
    }

    float d = sdPlane(p, vec4(0, 1, 0, 13));
    float wn = noise_2(p.xz + vec2(iTime, iTime*0.2));

    float t = mod(iTime, 15);
    floorColor *= smoothstep(0, 10, t);
    DistanceInfo floor = {d, wallType, floorColor};

    p = orgP;
    DistanceInfo jf = jellyfish(p, isMarch);

    if (isMarch)
        return floor;
    return sunk(floor, jf, 0.5);
}

float getReflectiveIndex(int type)
{
    if(type == wallType)
        return 0.0;
    if (type == sphereType)
        return 0.6;
    return 0.0;
}

vec3 eye = vec3(0);

vec3 getColor(in MarchResult result)
{
    vec3 jp = jellyfishPosition();
    jp = vec3(jp.y, -jp.x+10, -jp.z);
    vec3 lightPosition = jp;//vec3(6, -3, -5);
    if (result.type != invalidType) {
        vec3 ambient = result.color;
        
        vec3 invLight = normalize(lightPosition - result.position);
        vec3 normal = normal(result.position);
        float k = max(0.0, dot(result.rayDirection, reflect(invLight, normal)));
        float spec = 1 * pow(k, 50.0);

        float l = length(result.position.xz);
        float shadow = 1.0;
        if (result.type == wallType)
            shadow = shadowFunction(result.position, lightPosition, 32);
        
        float diffuse = max(0., dot(invLight, normal));
        vec3 color = vec3(ambient * (0.1 + 0.96*diffuse));
        float fog = exp(-0.00035*l*l);
        color += at;
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
    //fragColor = vec4(vec3(fragColor.y), 1.0);
}

)""
