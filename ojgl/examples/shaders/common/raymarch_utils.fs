R""(
const int invalidType = -1;

struct DistanceInfo {
    float distance;
    int type;
};

struct MarchResult {
    int type;
    vec3 position;
    int steps;
};

DistanceInfo map(in vec3 p);

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

MarchResult march(in vec3 rayOrigin, in vec3 rayDirection)
{
    float t = 0.0;
    MarchResult invalidResult = { invalidType, vec3(0.0), 0 };
    for (int steps = 0; steps < S_maxSteps; ++steps) {
        vec3 p = rayOrigin + t * rayDirection;
        DistanceInfo info = map(p);
        t += info.distance * S_distanceMultiplier;
        if (info.distance < S_distanceEpsilon)
            return MarchResult(info.type, p, steps);
        if (t > S_maxDistance)
            return invalidResult;
    }
    return invalidResult;
}
)""
