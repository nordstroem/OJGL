R""(
#include "edison2021/tunnel_base.fs"

DistanceInfo map(in vec3 p)
{
    DistanceInfo cylinder = {-sdCappedCylinder(p.xzy - tunnelDelta(p.z), vec2(2 + 0.1*filteredLines(10*atan(p.y), 1.1) , 50000)), wallType };
    DistanceInfo box = {-sdBox(p - tunnelDelta(p.z) + vec3(0, -1.2, 0.0), vec3(3, 2.0 + 0.0006*sin(7*p.x + 5*p.y + 5*p.z), 50000)), floorType };
    
    vec3 q = p - path(7.2);
    q.xz *= rot(sin(3*iTime)*cos(5*iTime));

    mo(q.xy, vec2(0.1, 0.3));
    q.x-=0.2;
    q.zy *= rot(iTime);
    mo(q.xy, vec2(0.4 + 0.2*sin(2*iTime), 0.09+ 0.2*cos(5*iTime)));
    q *= 1.0 + 0.3*sin(15*iTime)*cos(3*iTime);
    
    float dFrame = sdBoxFrame(q, vec3(0.3), 0.01);

    DistanceInfo thing = {dFrame, sphereType};
    
    DistanceInfo res = sunk(cylinder, box, 0.3);
    res = sunk(res, thing, 0.1);
    return res;
}


)""
