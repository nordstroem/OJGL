R""(
const int invalidType = -1;

struct DistanceInfo {
    float distance;
    int type;
    vec3 color;
};

struct MarchResult {
    int type;
    vec3 position;
    int steps;
    float transmittance;
    vec3 scatteredLight;
    int jump;
    vec3 rayDirection;
    vec3 color;
};

struct VolumetricResult {
    float distance;
    vec3 color;
};

vec3 firstJumpPosition = vec3(0);
DistanceInfo map(in vec3 p, bool isMarch, vec3 rd);
VolumetricResult evaluateLight(in vec3 p);
float getFogAmount(in vec3 p);
vec3 getColor(in MarchResult result);
float getReflectiveIndex(int type);

uniform int dummyVariable;
#define ZERO (min(dummyVariable,0))

vec3 normal(in vec3 p)
{
    //vec3 n = vec3(map(vec3(p.x + S_normalEpsilon, p.y, p.z), false).distance, map(vec3(p.x, p.y + S_normalEpsilon, p.z), false).distance, map(vec3(p.x, p.y, p.z + S_normalEpsilon), false).distance);
    //return normalize(n - map(p, false).distance);

    vec3 n = vec3(0.0);
    for( int i=ZERO; i<4; i++ )
    {
        vec3 e = 0.5773*(2.0*vec3((((i+3)>>1)&1),((i>>1)&1),(i&1))-1.0);
        n += e*map(p+0.0005*e, false, vec3(0)).distance;
      //if( n.x+n.y+n.z>100.0 ) break;
    }
    return normalize(n);

}

float shadowFunction(in vec3 hitPosition, in vec3 lightPosition, float k)
{
    float res = 1.0;

    float t = S_distanceEpsilon * 20.0;
    vec3 dir = lightPosition - hitPosition;
    float maxDistance = length(dir);
    dir = normalize(dir);
    while (t < maxDistance) {
        float h = map(hitPosition + dir * t, false, vec3(0)).distance;

        if(h < S_distanceEpsilon)
            return 0.0;

        res = min(res, k*h/t );

        t += h;
    }
    return res;
}

DistanceInfo un(DistanceInfo a, DistanceInfo b) { return a.distance < b.distance ? a : b; }
float smink( float a, float b, float k);

DistanceInfo sunk(DistanceInfo a, DistanceInfo b, float k) {
    DistanceInfo res = a.distance < b.distance ? a : b;
    res.distance = smink(a.distance, b.distance, k);
    return res;
}

vec3 march(in vec3 rayOrigin, in vec3 rayDirection)
{
    float t = 0.0;
    vec3 scatteredLight = vec3(0.0);
    float transmittance = 1.0;
    float reflectionModifier = 1.0;
    vec3 resultColor = vec3(0.0);
    int jump = 0;
#if S_REFLECTIONS
    for (; jump < S_reflectionJumps; jump++) {
#endif
        for (int steps = 0; steps < S_maxSteps; ++steps) {
            vec3 p = rayOrigin + t * rayDirection;
            if (jump == 0) {
                firstJumpPosition = p;
            }
            DistanceInfo info = map(p, true, rayDirection);
            float jumpDistance = info.distance * S_distanceMultiplier;

#if S_VOLUMETRIC
            float fogAmount = getFogAmount(p);
            VolumetricResult vr = evaluateLight(p);

            float volumetricJumpDistance = max(S_minVolumetricJumpDistance, vr.distance * S_volumetricDistanceMultiplier);
            jumpDistance = min(jumpDistance, volumetricJumpDistance);
            vec3 lightIntegrated = vr.color - vr.color * exp(-fogAmount * jumpDistance);
            scatteredLight += transmittance * lightIntegrated;
            transmittance *= exp(-fogAmount * jumpDistance);
#endif

            t += jumpDistance;
            if (info.distance < S_distanceEpsilon) {
                vec3 color = getColor(MarchResult(info.type, p, steps, transmittance, scatteredLight, jump, rayDirection, info.color));

#if !S_REFLECTIONS
                return color;
#else
                t = 0.0;
#if S_REFRACTIONS
                rayDirection = refract(rayDirection, normal(p), 0.7);
                rayOrigin = p + 0.5 * rayDirection;
#else
                rayDirection = reflect(rayDirection, normal(p));
                rayOrigin = p + 0.1 * rayDirection;

#endif
                resultColor = mix(resultColor, color, reflectionModifier);
                reflectionModifier *= getReflectiveIndex(info.type);
                break;
 #endif
            }

            if (t > S_maxDistance || steps == S_maxDistance - 1) {
                vec3 color = getColor(MarchResult(invalidType, p, steps, transmittance, scatteredLight, jump, rayDirection, info.color));
                resultColor = mix(resultColor, color, reflectionModifier);
                return resultColor;
            }
        }
#if S_REFLECTIONS
    }
#endif

    return resultColor;
}
)""
