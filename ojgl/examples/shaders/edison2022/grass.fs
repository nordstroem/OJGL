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

const int grassType = 1;
const int wallType = 2;
const int flowerType = 3;
const int lilyType = 4;

float at = 0.0;
float at2 = 0.0;

vec3 fogColor = vec3(0.0);

const float pi = 3.14159256;

float pcurve( float x, float a, float b )
{
    const float k = pow(a+b,a+b)/(pow(a,a)*pow(b,b));
    return k*pow(x,a)*pow(1.0-x,b);
}

VolumetricResult evaluateLight(in vec3 p)
{
    p.y += 10;
    pModPolar(p.xz, 8);
    float d = sdBox(p, vec3(19.0,  1*pow(psin(p.x + 5*iTime + 2*p.z),2), 0.1));
	d = max(0.01, d);

	float strength = 14;
	vec3 col = vec3(0.1, 0.8, 0.2);
	vec3 res = col * strength / (d * d);
	return VolumetricResult(d, res);
}


float getFogAmount(in vec3 p) {
    return 0.001;

}


float grassDistance(vec3 p, float h, float w, float d) 
{
    float a = pModPolar(p.xz, 4);
    //p.y+= 20 - t*20;
    p.x-=d;
    //float t2 = smoothstep(0.5, 1.0, t);
    //float k = -0.2 * (1-t2) + (0.8+0.2) * t2; 
    //p.x += 0.3*pow(0.15*(p.y+h), 2);
    p.x-= 0.8 * pow(0.15*(p.y+h), 2);
    float x = clamp((p.y + h)/(2*h), 0, 1);
    float r = w*max(0, pcurve(x, 0.2, 1.5));
    return sdBox(p.zyx, vec3(r, h, 0.2));
}

mat2 r05 = rot(0.5);

vec3 colorPalette(float t, vec3 a , vec3 b, vec3 c, vec3 d) {
    return a + b * cos(2 * pi * (c * t + d));
}


DistanceInfo jellyfish(in vec3 p, bool isMarch)
{
    p.xz *= rot(-0.2);
    float t = mod(iTime, 15);
    p.xz *= rot(0.2*sin(iTime));
    vec3 orgOctoP = p;

    float s = 0.6;
    float px = floor((p.y-s/2)/s);
    pMod1(p.y, s);
    
    float a = pModPolar(p.xz, 6);
    p.x -= 1.9;

    float ep = 0.12*px + 4*(mod(t-1.2, 2) - 1);
    p.x -= 0.3*sin(0.3*px + 3*iTime + 2*a) + 2*exp(-ep*ep);


    float d2 = sdBox(orgOctoP - vec3(0, -3.0, 0.0), vec3(8, 8, 8));
    float r = 0.2;
    float d = sdBox(p.xyz, vec3(r, s/2-0.01, r));
    d = max(d2, d);
    vec3 legColor = 1.0*vec3(0.5, 0.5, 1.0);
    legColor *= 0.2 + 0.5*psin(px*5);
    DistanceInfo legs = {0.5*d, flowerType, legColor };
    //if (isMarch)
    //    at += 0.0008/(1.2+d*d*d);

    p = orgOctoP;
    p.x -= 0.3*sin(0.3*p.y + 3*iTime);
    p.y -= 4.1;
    
    vec3 headColor = 20.0*vec3(0.2, 0.3, 1.0);

    ep = 4*(mod(t-1.2, 2)-1);
    ep = exp(-ep*ep);
    d = sdCutHollowSphere(vec3(p.x, -p.y-3.75*ep, p.z), 4+2*ep, -1-3*ep, 0.1);
    DistanceInfo head = {d, flowerType, headColor};
    if (isMarch)
        at += 0.3/(1.2+d*d*d);

    DistanceInfo blob = un(head, legs);

    return blob;

}

DistanceInfo map(in vec3 p, bool isMarch)
{
    vec3 grassColor = vec3(0.1, 0.5, 0.2);

    vec3 orgP = p;
    p = orgP;
    p.xz *= rot(0.5*iTime);
   // p.xy *= rot(0.6*iTime);

    float t0 = mod(iTime, 40);
    float t = smoothstep(2, 20, t0) - smoothstep(30, 40, t0);
    float r = 0.5 + 1.5*t;
    float ns = 1 - smoothstep(2, 20, t0);
    p.x -= ns*0.05*sin(20*iTime);
    p.y -= ns*0.05*sin(20*iTime + 30);
    p.y+= 10;
    p.y+= 4 - 10*t;
    float d = grassDistance(p, 20*t, r, 2);
    p.y -= 3.0 * t;
    p.xz *= r05;
    float d2 = grassDistance(p, 20*t, r, 1);
    if (d2 < d)
        grassColor *= 0.2;

    p.y -= 3.0 * t;
    p.xz *= r05;
    float d3 = grassDistance(p, 20*t, 1, 0.6);
    if (d3 < d2)
        grassColor *= 0.5;

    d = min(d, min(d2, d3));
        
    //p = orgP;
    //float d5 = sdCappedCylinder(p, vec2(0.2,10));
    //d = min(d, d5);

    //if (isMarch)
    //    fogColor += 0.07/(1.2+d*d*d) * grassColor;


    DistanceInfo grass = {d, grassType, grassColor};

    p = orgP;
    d = sdPlane(p, vec4(0, 1, 0, 13));
    float wn = noise_2(p.xz + vec2(iTime, iTime*0.2));
    vec3 floorColor = 14.67*vec3(0.0, 0.02, 0.05);

    DistanceInfo floor = {d + 0.03 * wn, wallType, 0.04*floorColor};
    
   // p.xz *= rot(0.1*sin(iTime));
    pModPolar(p.xz, 8);
    p.x -= 20;
    DistanceInfo jf = jellyfish(p, isMarch);

    p = orgP;



    return sunk(floor, jf, 0.5);
}

float getReflectiveIndex(int type)
{
    if(type == wallType)
        return 0.3;
    if (type == grassType)
        return 0.2;
    if (type == flowerType)
        return 0.8;
    return 0.0;
}

vec3 eye = vec3(0);

vec3 getColor(in MarchResult result)
{
    vec3 lightPosition = vec3(5, 15, 30);
    if (result.type != invalidType) {
        vec3 ambient = result.color;
        
        vec3 invLight = normalize(lightPosition - result.position);
        vec3 normal = normal(result.position);
        float k = max(0.0, dot(result.rayDirection, reflect(invLight, normal)));
        float spec = 1 * pow(k, 50.0);

        float l = length(result.position.xz);
        float shadow = 1.0;
        //if (result.type == wallType)
        //    shadow = shadowFunction(result.position, lightPosition, 32);
        
        float diffuse = max(0., dot(invLight, normal));
        vec3 color = vec3(ambient * (0.1 + 0.96*diffuse));
        color += fogColor;
        float fog = exp(-0.00035*l*l);
        color += at * 1.2*vec3(0.1, 0.1, 0.3);
        color *= (0.2 + 0.8*shadow) * fog ;
        return color * result.transmittance + result.scatteredLight;
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
    color = mix(color, 0.2*vec3(0.01, 0.1, 0.3), 0.25);
  //  color = mix(color, 0.2*vec3(0.01, 0.3, 0.1), 0.06);
    color /= (color + vec3(1.0));

    fragColor = vec4(pow(color, vec3(0.5)), 1.0);
    //fragColor = vec4(vec3(fragColor.y), 1.0);
}

)""
