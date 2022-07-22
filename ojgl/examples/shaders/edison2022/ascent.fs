R""(
#include "common/noise.fs"
#include "common/primitives.fs"
#include "edison2022/raymarch_settings_refract.fs"
#include "common/raymarch_utils.fs"
#include "common/utils.fs"

in vec2 fragCoord;
out vec4 fragColor;

uniform float iTime;
uniform vec2 iResolution;
uniform mat4 iCameraMatrix;
uniform float C_4_TOTAL;

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

int primes[8] = int[8](2, 3, 5, 7, 11, 13, 17, 19);
int order[6] = int[6](23, 2*11, 2*11*3*13, 2*11*3*13*5*17, 2*11*3*13*5*17*7*19, 2*11*3*13*5*17*7*19);

float powFunc(float left, float right, float value) {
    float t = (value - left) / (right - left);
    return pow(clamp(t, 0, 1), 5);
}

VolumetricResult evaluateLight(in vec3 p)
{
    vec3 orgP = p;
   
    p.y += 12.3;
    //float ep = 4*(mod(iTime-1.2, 2) - 1);
    //p.y += 1*exp(-ep*ep);

    int a = int(pModPolar(p.xz, 8)) + 3;
    float d = sdBox(p, vec3(19.0,  0.6*pow(0.7*psin(p.x + 5*iTime + 2*p.z),1), 0.001));


    if (iTime > 30) {
        float d3 = sdTorus(orgP + vec3(0, 12.3, 0), vec2(19.f, 0.001f));
        d3 = max(6 * (1-smoothstep(29.0, 33.0, iTime)), d3);
        float d2 = sdTorus(p, vec2(4.f, 0.2f));
        d2 = max(6 * (1-smoothstep(29.0, 33.0, iTime)), d2);
       // d = min(d, d2);
        d = min(d, d3);
    }
    d = max(0.01, d);

    float timeInterval = 4;
    int tk = iTime < 8 ? 0 : min(int((iTime-8)/timeInterval), 5);

	float strength = 0;
	vec3 col = vec3(0.1, 0.8, 0.2);


    if (order[tk] % primes[a] == 0) {
        if (tk > 0 && order[tk - 1] % primes[a] != 0) {
            strength = 10*powFunc(3,timeInterval, mod(iTime-8, timeInterval));
            //strength *= clamp(19*mod(iTime, 2)-p.x, 0.f, 1.f);
        }
        else {
            strength = 10.f;
        }
    } 

    //strength *= (1 - smoothstep(33.0,39.5, iTime));
	vec3 res = col * strength / (d * d);

	return VolumetricResult(d, res);
}


float getFogAmount(in vec3 p) {
    return 0.001;

}

mat2 r05 = rot(0.5);

vec3 colorPalette(float t, vec3 a , vec3 b, vec3 c, vec3 d) {
    return a + b * cos(2 * pi * (c * t + d));
}


DistanceInfo jellyfish(in vec3 p, bool isMarch, bool light, int id)
{
    p.xz *= rot(-0.2);
    float t = iTime;
    p.xz *= rot(0.2*sin(iTime));
    p.y += 120.0 - 120*smoothstep(0, 10, t + 3*hash11(23*id + 1.1)) + 15;

    if (iTime > 30) {
        p.y += 20*smoothstep(30, 36, iTime);
        p.x -= 100*smoothstep(30, 36, iTime);
    }
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
    if (isMarch)
        at += 0.02/(1.2+d*d*d);

    p = orgOctoP;
    p.x -= 0.3*sin(0.3*p.y + 3*iTime);
    p.y -= 4.1;
    
    vec3 headColor = 20.0*vec3(0.2, 0.3, 1.0);

    ep = 4*(mod(t-1.2, 2)-1);
    ep = exp(-ep*ep);
    d = sdCutHollowSphere(vec3(p.x, -p.y-3.75*ep, p.z), 4+2*ep, -1-3*ep, 0.1);
    DistanceInfo head = {d, flowerType, headColor};
    if (isMarch) {
        float ls = light ? 2.2 : 0.2;
        at += ls/(3.2+d*d*d*d*d) * (1 - smoothstep(36,37, iTime));
    }
    DistanceInfo blob = un(head, legs);

    return blob;

}

DistanceInfo map(in vec3 p, bool isMarch)
{
    vec3 grassColor = vec3(0.1, 0.5, 0.2);

    vec3 orgP = p;
    p = orgP;
    p.y += 12.5;
    float d = sdBox(p, vec3(100, 0.1, 100));
    float wn = noise_2(p.xz + vec2(iTime, iTime*0.2));
    vec3 floorColor = 14.67*vec3(0.0, 0.02, 0.05);

    DistanceInfo floor = {d + 0.03 * wn, wallType, 0.00*floorColor};
    p = orgP;
   // p.xz *= rot(0.1*sin(iTime));
    int a = int(pModPolar(p.xz, 8)) + 3;

    p.x -= 30 - 7*smoothstep(0, 15, iTime);
    DistanceInfo jf = jellyfish(p, isMarch, int(C_4_TOTAL) % 8 == a, a);
    //if (int(C_4_TOTAL) % 8 == a) {
    //    jf.color = vec3(1, 15, 1);//vec3(10*psin(10*a), 10*psin(2*a + 10), 10*psin(5*a));
    //}
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
        return 0.0;
    return 0.0;
}

vec3 eye = vec3(0);

vec3 getColor(in MarchResult result)
{
    vec3 lightPosition = vec3(5, 15, 30);
    vec3 lightColor = vec3(0.1, 0.3, 1.3);

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
        color += at * 1.2*lightColor;
        color *= (0.2 + 0.8*shadow) * fog;
        return color * result.transmittance + result.scatteredLight;
    } else {
        vec3 color = vec3(0);//0.04*14.67*vec3(0.0, 0.02, 0.05) * (1-smoothstep(20,34, iTime));
        //color += at * 1.2*vec3(0.1, 0.1, 0.3);
        float l = length(result.position.xz);
        float fog = exp(-0.00035*l*l);
        color += at * 1.2*lightColor;

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
   // color = mix(color, 0.2*vec3(0.01, 0.1, 0.3), 0.25);
  //  color = mix(color, 0.2*vec3(0.01, 0.3, 0.1), 0.06);
    color /= (color + vec3(1.0));

    fragColor = vec4(pow(color, vec3(0.5)), 1.0);

    //fragColor.xyz *= 1.0 - smoothstep(34., 35., iTime);
    
    //fragColor = vec4(vec3(fragColor.y), 1.0);
}

)""
