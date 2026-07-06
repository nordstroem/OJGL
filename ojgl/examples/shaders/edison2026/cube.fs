R""(
const float S_distanceEpsilon = 2e-3;
const float S_normalEpsilon = 1e-2;
const int S_maxSteps = 100;
const float S_maxDistance = 400.0;
const float S_distanceMultiplier = 1.0;
const float S_minVolumetricJumpDistance = 0.02;
const float S_volumetricDistanceMultiplier = 0.75;
const int S_reflectionJumps = 1;

#define S_VOLUMETRIC 0
#define S_REFLECTIONS 0

float sdBox(vec3 p, vec3 b)
{
    vec3 d = abs(p) - b;
    return length(max(d, 0.0)) + min(max(d.x, max(d.y, d.z)), 0.0);
}


const int invalidType = -1;

struct DistanceInfo {
    float distance;
    int type;
};

struct MarchResult {
    int type;
    vec3 position;
    int steps;
    float transmittance;
    vec3 scatteredLight;
    int jump;
};

struct VolumetricResult {
    float distance;
    vec3 color;
};

DistanceInfo map(in vec3 p);
VolumetricResult evaluateLight(in vec3 p);
float getFogAmount(in vec3 p);
vec3 getColor(in MarchResult result);
float getReflectiveIndex(int type);

vec3 normal(in vec3 p)
{
    vec3 n = vec3(map(vec3(p.x + S_normalEpsilon, p.y, p.z)).distance, map(vec3(p.x, p.y + S_normalEpsilon, p.z)).distance, map(vec3(p.x, p.y, p.z + S_normalEpsilon)).distance);
    return normalize(n - map(p).distance);
}

float shadowFunction(in vec3 hitPosition, in vec3 lightPosition, float k)
{
    float res = 1.0;

    float t = S_distanceEpsilon * 10.0;
    vec3 dir = lightPosition - hitPosition;
    float maxDistance = length(dir);
    dir = normalize(dir);
    while (t < maxDistance) {
        float h = map(hitPosition + dir * t).distance;

        if(h < S_distanceEpsilon)
            return 0.0;
        
        res = min( res, k*h/t );

        t += h;
    }
    return res;
}

DistanceInfo un(DistanceInfo a, DistanceInfo b) { return a.distance < b.distance ? a : b; }

vec3 march(in vec3 rayOrigin, in vec3 rayDirection)
{
    float t = 0.0;
    vec3 scatteredLight = vec3(0.0);
    float transmittance = 1.0;
    float reflectionModifier = 1.0;
    vec3 resultColor = vec3(0.0);

#if S_REFLECTIONS
    for (int jump = 0; jump < S_reflectionJumps; jump++) {
#else
        int jump = 0;
#endif
        for (int steps = 0; steps < S_maxSteps; ++steps) {
            vec3 p = rayOrigin + t * rayDirection;
            DistanceInfo info = map(p);
            float jumpDistance = info.distance * S_distanceMultiplier;

#if S_VOLUMETRIC
            float fogAmount = getFogAmount(p);
            VolumetricResult vr = evaluateLight(p);

            float volumetricJumpDistance = max(S_minVolumetricJumpDistance, vr.distance * S_volumetricDistanceMultiplier);
            jumpDistance = min(jumpDistance, volumetricJumpDistance);
            vec3 lightIntegrated = vr.color - vr.color * exp(-fogAmount * jumpDistance);
            lightIntegrated = max(vec3(0), lightIntegrated); // To fix the small black squares that could appear sometimes with bright volumetric light
            scatteredLight += transmittance * lightIntegrated;	
            transmittance *= exp(-fogAmount * jumpDistance);      
#endif

            t += jumpDistance;
            if (info.distance < (S_distanceEpsilon)) {
                vec3 color = getColor(MarchResult(info.type, p, steps, transmittance, scatteredLight, jump));
#if !S_REFLECTIONS
                return color;
#else
                resultColor = mix(resultColor, color, reflectionModifier);
                reflectionModifier *= getReflectiveIndex(info.type);
                if (reflectionModifier < 0.005) // Further reflections contribute nothing; skip the extra march (and the normal() below)
                    return resultColor;

                t = 0.0;
                rayDirection = reflect(rayDirection, normal(p));
                rayOrigin = p + 0.1 * rayDirection;

                break;
 #endif
            }

            if (t > S_maxDistance || steps == S_maxSteps - 1) {
                vec3 color = getColor(MarchResult(invalidType, p, steps, transmittance, scatteredLight, jump));
                resultColor = mix(resultColor, color, reflectionModifier);
                return resultColor;
            }
        }
#if S_REFLECTIONS
    }
#endif

    return resultColor;
}

#define PI 3.14159265
float pMod1(inout float p, float size)
{
    float halfsize = size * 0.5;
    float c = floor((p + halfsize) / size);
    p = mod(p + halfsize, size) - halfsize;
    return c;
}
vec2 pMod2(inout vec2 p, vec2 size)
{
    vec2 c = floor((p + size * 0.5) / size);
    p = mod(p + size * 0.5, size) - size * 0.5;
    return c;
}
vec3 pMod3(inout vec3 p, vec3 size)
{
    vec3 c = floor((p + size * 0.5) / size);
    p = mod(p + size * 0.5, size) - size * 0.5;
    return c;
}
float pModPolar(inout vec2 p, float repetitions)
{
    float angle = 2 * PI / repetitions;
    float a = atan(p.y, p.x) + angle / 2.;
    float r = length(p);
    float c = floor(a / angle);
    a = mod(a, angle) - angle / 2.;
    p = vec2(cos(a), sin(a)) * r;
    if (abs(c) >= (repetitions / 2))
        c = abs(c);
    return c;
}
mat2 rot(float a)
{
    return mat2(cos(a), sin(a), -sin(a), cos(a));
}
mat3 rotateAngle(vec3 v, float a)
{
    float si = sin(a);
    float co = cos(a);
    float ic = 1.0 - co;
    return mat3(v.x * v.x * ic + co, v.y * v.x * ic - si * v.z, v.z * v.x * ic + si * v.y, v.x * v.y * ic + si * v.z, v.y * v.y * ic + co, v.z * v.y * ic - si * v.x, v.x * v.z * ic - si * v.y, v.y * v.z * ic + si * v.x, v.z * v.z * ic + co);
}

void mo(inout vec2 p, vec2 d)
{
    p = abs(p) - d;
    if (p.y > p.x) p = p.yx;
}

float psin(float v){
	return 0.5 * (1.0 + sin(v));
}
float smink( float a, float b, float k )
{
    float h = clamp( 0.5+0.5*(b-a)/k, 0.0, 1.0);
    return mix( b, a, h ) - k*h*(1.0-h);
}

float beat(float time, float beatTime, float strength)
{
    float dt = time - beatTime;
    if (dt >= -0.5 && dt <= 0.5)
        return pow(psin(2 * PI * (dt - 0.5) - PI / 2), strength);
    return 0.0;
}

float cbeat(float time, float period, float strength)
{
    return pow(psin(2 * PI / period * (time - 0.5 * period) - PI / 2), strength);
}

in vec2 fragCoord;
out vec4 fragColor;

uniform float iTime;
uniform vec2 iResolution;
uniform mat4 iCameraMatrix;

const int cubeType = 1;

DistanceInfo map(in vec3 p)
{
    p.z += 5.0;
    p.xy *= rot(iTime);
    p.yz *= rot(iTime * 0.5);
    return DistanceInfo(sdBox(p, vec3(1.0)), cubeType);
}

float getReflectiveIndex(int type)
{
    return 0.0;
}

vec3 getColor(in MarchResult result)
{
    if (result.type != invalidType) {
        vec3 lightPosition = vec3(3.0, 4.0, -3.0);
        vec3 normal = normal(result.position);
        vec3 invLight = normalize(lightPosition - result.position);
        float diffuse = max(0.0, dot(invLight, normal));
        return vec3(0.2, 0.5, 0.9) * (0.1 + 0.9 * diffuse);
    } else {
        return vec3(0.0);
    }
}

VolumetricResult evaluateLight(in vec3 p)
{
    return VolumetricResult(1e6, vec3(0.0));
}

float getFogAmount(in vec3 p)
{
    return 0.0;
}

void main()
{
    float u = (fragCoord.x - 0.5);
    float v = (fragCoord.y - 0.5) * iResolution.y / iResolution.x;
    vec3 rayOrigin = (iCameraMatrix * vec4(u, v, -1.0, 1.0)).xyz;
    vec3 eye = (iCameraMatrix * vec4(0.0, 0.0, 0.0, 1.0)).xyz;
    vec3 rayDirection = normalize(rayOrigin - eye);

    vec3 color = march(rayOrigin, rayDirection);

    fragColor = vec4(pow(color, vec3(0.4545)), 1.0);
}
)""
