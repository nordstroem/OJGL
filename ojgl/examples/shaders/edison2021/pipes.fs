R""(
#include "edison2021/tunnel_base.fs"

uniform float drum;
uniform float[15] channel1;

const int pipesAType = lastBaseType + 1;
const int pipesBType = lastBaseType + 2;
const int starType = lastBaseType + 3;
const int cubeType = lastBaseType + 4;

const float fadeOutStartTime = 25;

VolumetricResult evaluateLight(in vec3 p)
{
    DistanceInfo res = {9999999, 0};

    float strength = 1.0;

    if (iTime < fadeOutStartTime - 2) {
        // Star
        const float distance = 40.0;
        vec3 starP = p;
        starP.z = mod(starP.z +  (iTime + 0.01) * 20, distance) - distance * 0.5;
        starP = (starP - vec3(tunnelDelta(p.z).x, 0, 0)).xzy;

        float r = 1.1 + sin(10*atan(starP.z, starP.x) + iTime * 25.0) * 0.2 + sin(iTime * 10.0) * 0.2;
        r *= 0.5;

        //res = DistanceInfo(sdTorus(starP, vec2(r, 0.0)), starType);

        starP = starP.xzy;
        //starP.y -= 0.3;
        vec2 q = starP.xy; //p.xy - starP.xy;
        float f = pModPolar(q, 15);
        f += 7;
        //starP.xy = -p.xy - q;

        //strength *= 1.0 + max(0.0, 1.0 - channel1[int(f)] * 3.0) * 50.0;

        //strength = f;


        float l = 0.3 - channel1[int(f)];
        l = max(0.1, l);
        float d = sdCappedCylinder(vec3(q.x - 0.7, q.y, starP.z), vec2(l, 0.0));
        //float d = length(vec3(q.x - 0.5, q.y, starP.z)) - 0.2;
        res = DistanceInfo(d, starType);
    }

    const int pipeStartTime = 10;

    if (iTime > pipeStartTime) { // Pipes
        float since = iTime - pipeStartTime;
        const float spinFrequency = 1.0;
        float pipeAdis = sdCylinder(p - vec3(tunnelDelta(p.z).x + sin(p.z * spinFrequency)        * 0.2, 0.0 +  cos(p.z * spinFrequency)        * 0.2, 0), 0.0);
        float pipeBdis = sdCylinder(p - vec3(tunnelDelta(p.z).x + sin(p.z * spinFrequency + 3.14) * 0.2, 0.0 +  cos(p.z * spinFrequency + 3.14) * 0.2, 0), 0.0);
        res = un(res, DistanceInfo(pipeAdis, pipesAType));
        res = un(res, DistanceInfo(pipeBdis, pipesBType));
    }



    vec3 col = vec3(0);
    if (res.type == starType) {
        strength *= 1;
    } else {
        strength = smoothstep(pipeStartTime, pipeStartTime + 5, iTime);
        // drum sync
    strength *= 1.0 + max(0.0, 1.0 - drum * 3.0) * 50.0;
    }

    float d = max(0.001, res.distance);

    col = vec3(0.1, 0.1, 1.0);
    strength *= 1.0 - smoothstep(fadeOutStartTime, 29, iTime);




	vec3 res2 = col * strength / (d * d);
	return VolumetricResult(d, res2);
}

float getFogAmount(in vec3 p)
{
    return 0.0005 + smoothstep(20, fadeOutStartTime, iTime);
}

float getReflectiveIndex(int type)
{
    if (type == wallType)
        return 0.05;
    if (type == pipesAType || type == pipesBType)
        return 0.8;
    if (type == starType)
        return 0.8;
    return 0.0;
}

vec3 getAmbientColor(int type, vec3 pos)
{
    vec3 wall = 0.6*vec3(0.2, 0.2, 0.2);
    if (type == wallType || type == floorType){
        return wall;
    }
    if (type == pipesAType) {
        return vec3(1, 0, 1);
    }
    if (type == pipesBType) {
        return vec3(0, 1, 1);
    }
    if (type == starType) {
        return 30 * vec3(1, 1, 1);
    }
    return vec3(0.1);
}

DistanceInfo map(in vec3 p)
{
    DistanceInfo cylinder = {-sdCappedCylinder(p.xzy - tunnelDelta(p.z) - vec3(0, 0., 0.0), vec2(1 + 0.1*filteredLines(10*atan(p.y), 1.1), 50000)), wallType };
	DistanceInfo res =  cylinder;

    return res;
}

)""
