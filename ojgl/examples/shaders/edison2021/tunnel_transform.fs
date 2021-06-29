R""(
#include "edison2021/tunnel_base.fs"

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
    return 0.0;
}


vec3 getAmbientColor(int type, vec3 pos) 
{
    vec3 wall = 1.0*palette(0.05*pos.z, vec3(0.5), vec3(0.5), vec3(1.0, 1.0, 1.0), vec3(0.0, 0.33, 0.67));
    if (type == wallType){
        return vec3(1.0, 0.2, psin(iTime));
    }

    return vec3(20.5);
}

DistanceInfo map(in vec3 p)
{
    float b = cbeat(iTime, 5.0, 2.0);
    DistanceInfo cylinder = {-sdCappedCylinder(p.xzy - tunnelDelta(p.z), vec2(1.0 + 0.1*filteredLines(10*atan(p.y), 1.1), 50000)), wallType };
    DistanceInfo floorBox = {-sdBox(p - tunnelDelta(p.z + 3*b) + vec3(0, -0.0, 0.0), vec3(0.8 + 0.5*b, + 0.8 + 0.5*b + 0.0006*sin(7*p.x + 5*p.y + 5*p.z), 50000)), wallType};
    
    DistanceInfo tunnel = sunk(cylinder, floorBox, 0.1);


    DistanceInfo box = {sdSphere(p - path(-3.5),  0.1 + 0.05 * cbeat(iTime, 0.4, 3.0)), sphereType };
    
    return tunnel;//un(cylinder, box);
}


)""
