R""(

const float S_distanceEpsilon = 1e-3;
const float S_normalEpsilon = 1e-3;
const int S_maxSteps = 150;
const float S_maxDistance = 400.0;
const float S_distanceMultiplier = 0.5;
const float S_minVolumetricJumpDistance = 0.03;
const float S_volumetricDistanceMultiplier = 0.75;
const int S_reflectionJumps = 2;

#define S_VOLUMETRIC 1
#define S_REFLECTIONS 1
#define S_REFRACTIONS 0

#include "common/noise.fs"
#include "common/primitives.fs"
#include "common/raymarch_utils.fs"
#include "common/utils.fs"

in vec2 fragCoord;
out vec4 fragColor;

uniform float iTime;
uniform vec2 iResolution;
uniform mat4 iCameraMatrix;

uniform float C_6_SINCE; // 6 melody synth
uniform float C_6_TOTAL;

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


VolumetricResult volUn(VolumetricResult a, VolumetricResult b)
{
    return VolumetricResult(min(a.distance, b.distance), a.color + b.color);
}

const float missileHeight = 20;//15 + 10 * smoothstep(15, 20, iTime);
const float missileRadius = 20;
#define MISSILE_TIME (C_6_TOTAL * 0.5 + 5 + 0.5 * pow(C_6_SINCE, 0.3))
//(0.75 * (C_6_TOTAL * 0.5  + 0.5 * pow(C_6_SINCE, 0.3)))

vec3 ballPositionCalc() {
    float t = MISSILE_TIME;//4*mod(iTime, 10);
    const float speed = 25;
    const float perimeter = 2 * pi * missileRadius;
    const float endAngle = 4*pi - 2.2;
    const float circleAngleLength =  (2  + ((2*pi - 2.2) / (2 * pi)));
    float t0 = 5;
    float t1 = t0 + (22 + missileHeight) / speed;
    float t2 = t1 + missileRadius / speed;
    float t3 = t2 + circleAngleLength * perimeter / speed;


    if (t < t0) { // hidden
        return vec3(0, -12 - 10 -10*smoothstep(0, t0, t), 0);
    } else  if (t < t1){ // up
        return vec3(0, -22 + (t-t0) * speed, 0);
    } else if (t < t2) { // out to torus
         return vec3(0, missileHeight, speed*(t-t1));
    } else if (t < t3) {
        float angle = (t-t2) / (t3-t2) *  2 * pi * circleAngleLength;
        return vec3(missileRadius*sin(angle), missileHeight, missileRadius * cos(angle));
    } else { // escape
        return vec3(missileRadius*sin(endAngle) + (t - t3) * speed * sin(endAngle), missileHeight, missileRadius * cos(endAngle)  + (t - t3) * speed * cos(endAngle));
    }

    return vec3(0, 0, 0);
}

vec3 ballPosition = ballPositionCalc();

VolumetricResult missile(in vec3 p) {
    float t = MISSILE_TIME;//4*mod(iTime, 10);
    const float speed = 25;
    const float perimeter = 2 * pi * missileRadius;
    const float endAngle = 4*pi - 2.2;
    const float circleAngleLength =  (2  + ((2*pi - 2.2) / (2 * pi)));
    float t0 = 5;
    float t1 = t0 + (22 + missileHeight) / speed;
    float t2 = t1 + missileRadius / speed;
    float t3 = t2 + circleAngleLength * perimeter / speed;

    vec3 orgP = p;
	vec3 col = vec3(0.1, 0.8, 0.2);
    float strength = 20;

    float distortionStrength = 0.5;
    float distortionFrequency = 1.0;

    float snakeRadius = 0.2;
    float verticalD = sdCappedCylinder(p - vec3(0, 0, distortionStrength*sin(distortionFrequency*p.y)), vec2(snakeRadius, missileHeight));
    float horizontalD = sdCappedCylinder(p.xzy - vec3(distortionStrength*sin(distortionFrequency*p.z), +missileRadius/2, missileHeight), vec2(snakeRadius, missileRadius/2));
    if (t > t2 + 2) {
        horizontalD = 99999.0;
    }

    float angle = atan(p.x, p.z);
    float torusD = sdTorus(p - vec3(0, missileHeight, 0), vec2(missileRadius + distortionStrength*sin(distortionFrequency*angle * 25.0) * 2.0, snakeRadius));
    if (t > t2 - 1.0 && t < t2 + 1.0) {
        torusD = max(torusD, -p.x);
    }


    p.xz *= rot(2.2);
    float escapeD = 999999.0;
    if (t > t3 - 1.0) {
        escapeD = sdCappedCylinder(p.xzy - vec3(distortionStrength*sin(distortionFrequency*p.z), missileRadius+150 + 1.5, missileHeight), vec2(snakeRadius, 150));
        torusD = max(torusD, p.x);
    }

    //torusD = max(0.05, torusD);



    p = orgP;
    float d = min(verticalD, horizontalD);//min(verticalD, torusD);
    d = min(d, torusD);
    d = min(d, escapeD);
    d = max(0.01, d);
    float ball = length(orgP - ballPosition) - 7.0;

    //d = min(ball, horizontalD);
    d = max(d, ball);
    //d = min(d, ball);

    vec3 res = col * strength * smoothstep(0, 3, iTime) / (d * d);
    VolumetricResult bullet = {d, res};
    return bullet;
}

VolumetricResult evaluateLight(in vec3 p)
{
    vec3 orgP = p;
    p.y += 12.;
    int a = int(pModPolar(p.xz, 8)) + 3;
    float d = sdBox(p, vec3(19.0,  0.6*pow(0.7*psin(p.x + 5*(iTime +34)+ 2*p.z),1), 0.1));
    float d2 = sdTorus(p, vec2(4.f, 0.2f));
    d = min(d, d2);

    float d3 = sdTorus(orgP + vec3(0, 12, 0), vec2(19.f, 0.2f));
    d = min(d, d3);
    d = max(0.01, d);

	float strength = 10 * (1 - smoothstep(0, 6, iTime));
	vec3 col = vec3(0.1, 0.8, 0.2);

	vec3 res = col * strength / (d * d);
    VolumetricResult symbols = {d, res};

	return volUn(symbols, missile(orgP));
}


float getFogAmount(in vec3 p) {
    return 0.001;

}

DistanceInfo map(in vec3 p, bool isMarch)
{
    vec3 grassColor = vec3(0.1, 0.5, 0.2);

    vec3 orgP = p;
    p = orgP;
   // p.xz *= rot(3.6*smoothstep(0, 20, iTime)  + 0.07*iTime);
    p.xz *= rot(0.6 * iTime);
    float t0 = iTime;
    float t = smoothstep(5, 20, t0) - smoothstep(30, 40, t0);
    float r = 0.5 + 1.5*t;
    float ns = 1 - smoothstep(2, 20, t0);
    p.x -= ns*0.05*sin(20*iTime);
    p.y -= ns*0.05*sin(20*iTime + 30);
    p.y+= 10;
    p.y+= 4 - 10*t;
    float d = grassDistance(p, 18*t, r, 2);
    p.y -= 3.0 * t;
    p.xz *= r05;
    float d2 = grassDistance(p, 18*t, r, 1);
    if (d2 < d)
        grassColor *= 0.2;

    p.y -= 3.0 * t;
    p.xz *= r05;
    float d3 = grassDistance(p, 18*t, 1, 0.6);
    if (d3 < d2)
        grassColor *= 0.5;

    d = min(d, min(d2, d3));

    //p = orgP;
    //float d5 = sdCappedCylinder(p, vec2(0.2,10));
    //d = min(d, d5);

    if (isMarch)
        fogColor += 0.07/(1.2+d*d*d) * grassColor * smoothstep(0, 5, iTime);


    DistanceInfo grass = {d, grassType, grassColor};

    p = orgP;
    p.y += 12.5;
    d = sdBox(p, vec3(600, 0.1, 600));
    float wn = noise_2(p.xz + vec2(iTime, iTime*0.2));
    vec3 floorColor = 14.67*vec3(0.0, 0.02, 0.05);

    DistanceInfo floor = {d + 0.03 * wn, wallType, 0.00*floorColor};


    return sunk(floor, grass, 0.5);
}

float getReflectiveIndex(int type)
{
    if(type == wallType)
        return 0.3 * smoothstep(0, 3, iTime);
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
        return color  * result.transmittance + result.scatteredLight;
    } else {
        vec3 color = 0.00*14.67*vec3(0.0, 0.02, 0.05);
        //color += at * 1.2*vec3(0.1, 0.1, 0.3);
        float l = length(result.position.xz);
        float fog = exp(-0.00035*l*l);

        //if (result.position.y < 100)
             color = color * fog * result.transmittance + result.scatteredLight;
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
    fragColor.xyz *= (1 - smoothstep(25, 32, iTime));
    //fragColor = vec4(vec3(fragColor.y), 1.0);


}

)""
