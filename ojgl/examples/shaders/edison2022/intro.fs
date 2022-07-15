R""(

const float S_distanceEpsilon = 1e-3;
const float S_normalEpsilon = 1e-3;
const int S_maxSteps = 100;
const float S_maxDistance = 250.0;
const float S_distanceMultiplier = 0.7;
const float S_minVolumetricJumpDistance = 0.02;
const float S_volumetricDistanceMultiplier = 0.75;
const int S_reflectionJumps = 2;

#define S_VOLUMETRIC 0
#define S_REFLECTIONS 0
#define S_REFRACTIONS 0

#include "common/noise.fs"
#include "common/primitives.fs"
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
const int shipType = 3;
const int ojType = 4;

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


vec3 jellyfishPosition_func() {
    vec3 p = vec3(0.f);
    float t = iTime;
    p.y+=20;
    p.z-=60;
    p.x-=-10;
    vec3 orgPos = p;
    p.y += 0.5*sin(iTime);
    p.y -= 0.5 * (2*t + 3*floor(t / 2) + 3*smoothstep(0, 1, mod(t, 2)));
    return p;
}

const vec3 jellyfishPosition = jellyfishPosition_func();

DistanceInfo jellyfish(in vec3 p, bool issMarch)
{
    p.x += 13;
    p.z += 40;
    p.y += -30;
    p.xz *= rot(-0.2);
    p = vec3(p.y, -p.x, p.z);//
    float t = iTime;
    p += jellyfishPosition;
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


float ojDistance(in vec3 p, bool isMarch) {
    p.x-=12.9;
    p.z-=0.9;
    p.y -= 4.3;
    p = vec3(p.y, -p.x, p.z);
    p.xy *= rot(-1.047);
    vec3 orgP = p;

    p.y *= 0.6;
    float d = sdTorus(p.yxz, vec2(2, 0.0));

    p = orgP;
   // d = min(d, sdBox(p, vec3(0.1, 4.5, 0.1)));
   // d = min(d, sdBox(p, vec3(0.1, 0.1, 3)));

    p.z -= -5;
    vec3 jp = p;
    p.y -= 3.4;
    d = min(d, sdBox(p, vec3(0.0, 0.1, 1)));

    p = jp;
    p.z-= -1;
    d = min(d, sdBox(p, vec3(0.0, 3.5, 0.1)));
    p = jp;
    p.z-= 0;
    p.y-=-2.5;
    p.zy *= rot(-0.8);
    d = min(d, sdBox(p, vec3(0.0, 1.5, 0.0)));


    if (isMarch)
        at += 0.02/(0.0+d*d*d*d) * smoothstep(7,13, iTime);
    return d;
}

DistanceInfo stone(in vec3 p, bool isMarch) {
    //p.y += 6;
    //p.xz *= rot(-1.2);
    //vec3 orgP = p;
    ////float d = sdBox(p, vec3(2.0, 2.0, 2.0));
    ////DistanceInfo stone = {d + fbm3_high(orgP*4.0,0.4,2.96 ) * 0.1, shipType, vec3(0.1)};


    //pModPolar(p.xz, 2.7);
    //mo(p.xy, vec2(0.0, 1.5));
    //float d = sdTriPrism(p, vec2(20.0, 10.0));// + fbm3_high(orgP*4.0,0.4,2.96 ) * 0.1 + 0.8*noise_3(p*0.5);

    //// d = min(d, ojd);

    // Ship
    const float scale = 40.0;

    vec3 rp = p / scale;
    rp.xz *= rot(-1.5);
    rp.zy *= rot(-1.0);
    const float len = 0.7;
    const float height = 0.1;
    float width = 0.2 + rp.y * 0.5;
    if (rp.x < -0.1) {
        width += rp.x + 0.1;
    }
    float d = sdRoundBox(rp , vec3(len, height, width), 0.0);
    d = max(-sdRoundBox(rp - vec3(0.55, 0.1, 0) , vec3(0.1, 0.1, 0.15), 0.0), d);

    // top
    if (rp.y > 0.2) {
        width -= rp.y - 0.2;
    }

    const float d2 = sdRoundBox(rp - vec3(0.1, height, 0), vec3(0.3, 0.2, width), 0.0);

    d = min(d, d2);

    // antenna
    const float w = 0.02 + 0.5 - rp.y * 0.2 - 0.4;
    const float d3 = sdRoundBox(rp - vec3(0, 0.22, 0), vec3(w, 0.3, w), 0.0);

    d = smink(d, d3, 0.05);

    d *= scale;
    // End ship


    float ojd = ojDistance(p - vec3(0.2, 2.5, -8), isMarch);

    //d = min(d, ojd);

     //d -= 0.05*noise_3(2.0*p);
     DistanceInfo stone = {d, shipType,  vec3(0.1)};
     DistanceInfo oj = {ojd, ojType,  vec3(0.1)};
    return un(stone, oj);
}






DistanceInfo map(in vec3 p, bool isMarch)
{
    vec3 orgP = p;
    p = orgP;
    vec2 ojUV = clamp(vec2(-p.z / 20 + 1, -p.x / 20 + 1), vec2(0.0), vec2(1.0));
   // bool onOJ = texture(ojText, ojUV).x < 0.5;
    vec3 floorColor = 0.967*vec3(0.0, 0.02, 0.05);

    //if (onOJ) {
    //    floorColor = 0.05*vec3(1.0);
    //   //floorColor *= 0.5;
    //   p.y-=0.5 +  0.1*noise_2(3*p.xz);
    //   //float d = sdPlane(p, vec4(0, 1, 0, 13));
    // //  floorColor += 0.01/(0.05+0.9*d*d*d) * vec3(3.0, 3.0, 0.4);
    //}

    float d = sdPlane(p, vec4(0, 1, 0, 13));
    //float wn = noise_2(p.xz + vec2(iTime, iTime*0.2));
    const float n = noise_2(p.xz);
    float wn = n*n + noise_2(p.xz * 0.3) * 2.0;

    //float t = mod(iTime, 15);
    //floorColor *= smoothstep(0, 10, t);
    //DistanceInfo floor = {d - 0.08*sin(p.x), wallType, floorColor};
    DistanceInfo floor = {d - wn * 2.0 - sin((p.x - 40.0) * 0.05)*20.0 - 20, wallType, floorColor};

    p = orgP;


    p.y+=3;
    DistanceInfo ojD = stone(p, isMarch);
    //ojD.distance -= 0.05*noise_3(2.0*p);
    DistanceInfo res = sunk(floor, ojD, 1);

    {
        //p = orgP;
        //pMod2(p.xz, vec2(30.0));
        //float d = sdCylinder(p.xzy, 3);
        //res = un(res, DistanceInfo(d, shipType, vec3(1, 0, 0)));
    }

    //res = un(res, DistanceInfo(90-p.y, wallType, vec3(0)));

    if (!isMarch) {
        DistanceInfo jf = jellyfish(orgP, isMarch);
        return un(res, jf);
    }

    return res;
}

float getReflectiveIndex(int type)
{
    if(type == wallType)
        return 0.0;
    if (type == sphereType)
        return 0.0;
    if (type == shipType)
        return 0.0;
    if (type == ojType)
        return 0.0;
    return 0.0;
}

float getSpecularIndex(int type)
{
    if(type == shipType)
        return 1.0;
    return 1.0;
}

vec3 eye = vec3(0);

vec3 getColor(in MarchResult result)
{
    //vec3 jp = jellyfishPosition();
    //jp = vec3(jp.y, -jp.x+10, -jp.z);
    vec3 lightPosition = vec3(0, 30, 20 - 5.0 * max(0, iTime - 9)); //vec3(-15, 35, -10);
    if (result.type != invalidType) {
        vec3 ambient = result.color;
        vec3 normal = normal(result.position);
        if (result.type == wallType) {
            ambient = mix(vec3(0,0.05,0), 0.967*vec3(0.0, 0.02, 0.05), normal.y);
        }

        vec3 invLight = normalize(lightPosition - result.position);
        float k = max(0.0, dot(result.rayDirection, reflect(invLight, normal)));
        float spec = getSpecularIndex(result.type) * pow(k, 5.0);

        float l = length(result.position.xz);
        float shadow = 1.0;
       //z if (result.type == wallType)
            shadow = shadowFunction(result.position, lightPosition, 32);

        float diffuse = max(0., dot(invLight, normal));
        vec3 color = vec3(ambient * (0.00 + 0.96*diffuse)) * 1.0;
        float fog = exp(-0.00035*l*l);
        color += at;//*at*200.0;
        color *= ((0.0 + 0.9*shadow)  + 30.0*spec*shadow) * fog;
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

    const float zoom = 1.0 - smoothstep(8, 11, iTime) * 0.3;
    u *= zoom;
    v *= zoom;


    vec3 rayOrigin = (iCameraMatrix * vec4(u, v, -0.7, 1.0)).xyz;
    eye = (iCameraMatrix * vec4(0.0, 0.0, 0.0, 1)).xyz;
    vec3 rayDirection = normalize(rayOrigin - eye);

    vec3 color = march(rayOrigin, rayDirection);

    // Tone mapping
    color = mix(color, 0.1*vec3(0.01, 0.1, 0.3), 0.2);
    color /= (color + vec3(1.0));

    fragColor = vec4(pow(color, vec3(0.5)), 0.3 + 0.5 * noise_2(vec2(u, v + iTime * 0.1) * 50.0));
    //fragColor = vec4(vec3(fragColor.y), 1.0);

    fragColor.xyz *= smoothstep(0, 6, iTime);
}

)""
