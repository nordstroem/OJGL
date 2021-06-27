R""(
#include "edison2021/tunnel_base.fs"

const int blobType = lastBaseType + 1;

float tunnelDeltaModifier() {
    return 3;
}

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
    if (type == blobType)
        return 0.5;
    return 0.0;
}

vec3 getAmbientColor(int type, vec3 pos) 
{
    vec3 wall = 0.9*vec3(0.2, 0.2, 0.2); 
    if (type == wallType || type == floorType){
        return wall;
    }
    if (type == blobType) {
        return 5*vec3(0.1, 1.0, 0.1);
	}
    return vec3(0.1);
}

DistanceInfo map(in vec3 p)
{
    DistanceInfo cylinder = {-sdCappedCylinder(p.xzy - tunnelDelta(p.z) - vec3(0, 0., 0.0), vec2(1 + 0.1*filteredLines(10*atan(p.y), 1.1), 50000)), wallType };
    DistanceInfo floorBox = {-sdBox(p - tunnelDelta(p.z) + vec3(0, -0.9, 0.0), vec3(2, 1.4 + 0.0006*sin(7*p.x + 5*p.y + 5*p.z), 50000)), floorType };
    DistanceInfo tunnel = sunk(cylinder, floorBox, 0.1);

    
    vec3 orgP = p;
    p = orgP.xzy - vec3(0.0, path(-5.5).z, 0.0);
    float wormRadius = 3.0 * smoothstep(0.0, 10.0, iTime);
    float r = wormRadius - (p.y + wormRadius )/2;
    DistanceInfo blob = {sdCappedCylinder(p - tunnelDelta(orgP.z) + 0.1*sin(3*p.x) - vec3(0, 0, -0.8 - -0.1*r + 0.5*psin(5 * r + 10* iTime)), vec2(0.03 + 0.04*r*r, wormRadius)), blobType};
    
    return sunk(tunnel, blob, 0.1);
}


)""
