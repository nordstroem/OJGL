R""(
#include "edison2021/tunnel_base.fs"

DistanceInfo map(in vec3 p)
{
    DistanceInfo cylinder = {-sdCappedCylinder(p.xzy - tunnelDelta(p.z), vec2(2 + 0.1*filteredLines(10*atan(p.y), 1.1) , 50000)), wallType };
    DistanceInfo floorBox = {-sdBox(p - tunnelDelta(p.z) + vec3(0, -1.2, 0.0), vec3(3, 2.0 + 0.0006*sin(7*p.x + 5*p.y + 5*p.z), 50000)), floorType };
   
    p = p - path(-5.5);
    p.y -= 0.3*sin(iTime);
    p.zy *= rot(-0.15);
    
    p = p.xzy;

    DistanceInfo b0 = {sdVerticalCapsule(p - vec3(0, 0, -0.035), 0.3, 0.02), sphereType};

    p.x = abs(p.x);
    
    p.xz *= rot(0.5*sin(7*iTime));

    DistanceInfo b1 = {sdBox(p, vec3(0.4, 0.2, 0.005)), sphereType};
    

    p = p - vec3(0.6, 0, 0);
    p.x+=0.2;
    p.xz *= rot(0.6*sin(7*(iTime - 0.15)));
    p.x-=0.2;

    float tipWidth = 0.2;
    float r = clamp(1-(p.x+tipWidth)/(tipWidth*2), 0, 1);

    DistanceInfo b2 = {sdBox(p, vec3(tipWidth, 0.18*r, 0.001)), sphereType};
    DistanceInfo bird = sunk(b0, sunk(b1, b2, 0), 0.15);

    DistanceInfo res = sunk(cylinder, floorBox, 0.3);
    res = sunk(res, bird, 0.1);
    return res;
}


)""
