R""(
#include "edison2021/tunnel_base.fs"

DistanceInfo map(in vec3 p)
{
    DistanceInfo cylinder = {-sdCappedCylinder(p.xzy - tunnelDelta(p.z), vec2(2 + 0.1*filteredLines(10*atan(p.y), 1.1) , 50000)), wallType };
    DistanceInfo floorBox = {-sdBox(p - tunnelDelta(p.z) + vec3(0, -1.2, 0.0), vec3(3, 2.0 + 0.0006*sin(7*p.x + 5*p.y + 5*p.z), 50000)), floorType };
    DistanceInfo res = sunk(cylinder, floorBox, 0.3);

    { // Pipes
        const float spinFrequency = 1.0;
        float pipeAdis = sdCylinder(p - vec3(tunnelDelta(p.z).x + sin(p.z * spinFrequency)        * 0.3, 0.0 +  cos(p.z * spinFrequency)        * 0.3, 0), 0.05);
        float pipeBdis = sdCylinder(p - vec3(tunnelDelta(p.z).x + sin(p.z * spinFrequency + 3.14) * 0.3, 0.0 +  cos(p.z * spinFrequency + 3.14) * 0.3, 0), 0.05);
        res = un(res, DistanceInfo(pipeAdis, pipesAType));
        res = un(res, DistanceInfo(pipeBdis, pipesBType));
    }

    { // Star
        const float distance = 30.0;
        vec3 starP = p;
        starP.z = mod(starP.z +  iTime * 30, distance) - distance * 0.5;
        starP = (starP - vec3(tunnelDelta(p.z).x, 0, 0)).xzy;

        float r = 1.1 + sin(10*atan(starP.z, starP.x) + iTime * 25.0) * 0.2 + sin(iTime * 10.0) * 0.2;
        float d = sdTorus(starP, vec2(r, 0.04));  
        
        res = un(res, DistanceInfo(d, starType));
    }

    return res;
}


)""
