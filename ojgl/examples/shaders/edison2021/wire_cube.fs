R""(
#include "edison2021/tunnel_base.fs"


const int cubeType = lastBaseType + 1;

VolumetricResult evaluateLight(in vec3 p)
{
	return VolumetricResult(1000, vec3(0));
}

float getFogAmount(in vec3 p) 
{
    return 0.0001;
}

float getReflectiveIndex(int type)
{
    if (type == wallType)
        return 0.02;
    if (type == cubeType)
        return 0.5;
    return 0.0;
}

vec3 getAmbientColor(int type, vec3 pos) 
{
    vec3 wall = 0.9*vec3(0.2, 0.2, 0.2); 
    if (type == wallType || type == floorType){
        return wall;
    }
    if (type == cubeType) {
        return 5*vec3(1.0);
	}
    return vec3(0.1);
}


DistanceInfo map(in vec3 p)
{
    DistanceInfo cylinder = {-sdCappedCylinder(p.xzy - tunnelDelta(p.z) - vec3(0, 0., 0.0), vec2(1 + 0.1*filteredLines(10*atan(p.y), 1.1), 50000)), wallType };
    DistanceInfo floorBox = {-sdBox(p - tunnelDelta(p.z) + vec3(0, -0.9, 0.0), vec3(2, 1.4 + 0.0006*sin(7*p.x + 5*p.y + 5*p.z), 50000)), floorType };
    DistanceInfo tunnel = sunk(cylinder, floorBox, 0.1);

    vec3 q = p - path(-7.2);
    q.xz *= rot(sin(3*iTime)*cos(5*iTime));

    mo(q.xy, vec2(0.1, 0.3));
    q.x-=0.2;
    q.zy *= rot(iTime);
    mo(q.xy, vec2(0.4 + 0.2*sin(2*iTime), 0.09+ 0.2*cos(5*iTime)));
    q *= 1.0 + 0.3*sin(15*iTime)*cos(3*iTime);
    
    float dFrame = sdBoxFrame(q, vec3(0.3), 0.01);

    DistanceInfo wireCube = {dFrame, cubeType};
    DistanceInfo res = sunk(tunnel, wireCube, 0.1);
    return res;
}


)""
