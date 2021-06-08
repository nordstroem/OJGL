R""(
#include "edison2021/tunnel_base.fs"

DistanceInfo map(in vec3 p)
{
    DistanceInfo cylinder = {-sdCappedCylinder(p.xzy - tunnelDelta(p.z), vec2(2 + 0.1*filteredLines(10*atan(p.y), 1.1) , 50000)), wallType };
    DistanceInfo floorBox = {-sdBox(p - tunnelDelta(p.z) + vec3(0, -1.2, 0.0), vec3(3, 2.0 + 0.0006*sin(7*p.x + 5*p.y + 5*p.z), 50000)), floorType };
   

    p = p - path(-2.5);
    p = p - vec3(-0.28, 0, 0);
    DistanceInfo b1 = {sdBox(p, vec3(0.02)), wallType};
    float r = clamp(p.x/0.2, 0, 1);

    DistanceInfo b2 = {sdBox(p, vec3(0.8, 0.2-0.2*r, 0.01)), sphereType};
    DistanceInfo bird = b2;

    DistanceInfo res = sunk(cylinder, floorBox, 0.3);
    res = un(res, bird);
    return res;
}


)""
