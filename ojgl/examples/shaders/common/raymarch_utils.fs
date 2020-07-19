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
    vec3 n = vec3(map(vec3(p.x + normalEpsilon, p.y, p.z)).distance, map(vec3(p.x, p.y + normalEpsilon, p.z)).distance, map(vec3(p.x, p.y, p.z + normalEpsilon)).distance);
    return normalize(n - map(p).distance);
}

DistanceInfo un(DistanceInfo a, DistanceInfo b) { return a.distance < b.distance ? a : b; }

MarchResult march(in vec3 rayOrigin, in vec3 rayDirection)
{
    float t = 0.0;
    MarchResult invalidResult = { invalidType, vec3(0.0), 0 };
    for (int steps = 0; steps < maxSteps; ++steps) {
        vec3 p = rayOrigin + t * rayDirection;
        DistanceInfo info = map(p);
        t += info.distance;
        if (info.distance < distanceEpsilon)
            return MarchResult(info.type, p, steps);
        if (t > maxDistance)
            return invalidResult;
    }
    return invalidResult;
}
)""
