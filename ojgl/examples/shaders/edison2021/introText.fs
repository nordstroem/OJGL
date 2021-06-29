R""(
#include "edison2021/tunnel_base.fs"

uniform sampler2D pirateyTexture;

const int textType = lastBaseType + 1;

VolumetricResult evaluateLight(in vec3 p)
{
	return VolumetricResult(1000, vec3(0));
}

float getFogAmount(in vec3 p) 
{
    return 0.0001;
}


float uvBox(vec3 p, vec3 b, inout vec2 uv)
{
    vec3 d = abs(p) - b;
    float dis = length(max(d, 0.0)) + min(max(d.x, max(d.y, d.z)), 0.0);

    uv = p.xy / (b.xy * 2) - 0.5;
    uv.x*=-1;
    return dis;
}

DistanceInfo pirateyText(in vec3 p)
{
    vec2 uv;
    DistanceInfo box = {uvBox(p, vec3(0.5, 0.2, 0.0), uv), textType };
    uv.x *=-1;
    if (true && box.distance < S_distanceEpsilon) {
        float s = texture(pirateyTexture, uv).x;
        if (s > 0.2) { // If not on text
            box.type = textType;
            box.distance = 5000000;
        }
	}
        
    return box;
}

float getReflectiveIndex(int type)
{
    if (type == wallType)
        return 0.02;
    if (type == floorType)
        return 0.2;
    if (type == textType)
        return 0.1;
    return 0.0;
}

vec3 getAmbientColor(int type, vec3 pos) 
{
    vec3 wall = 0.9*vec3(0.2, 0.2, 0.2); 
    if (type == wallType || type == floorType){
        return wall;
    }
    if (type == textType) {
        return 50*vec3(1.0);
	}
    return vec3(0.1);
}

DistanceInfo map(in vec3 p)
{
    vec3 p2 = p.xzy - tunnelDelta(p.z);
    DistanceInfo cylinder = {-sdCappedCylinder(p.xzy - tunnelDelta(p.z) - vec3(0, 0., 0.0), vec2(1 + 0.1*filteredLines(10*atan(p.y), 1.1), 50000)), wallType };
    DistanceInfo floorBox = {-sdBox(p - tunnelDelta(p.z) + vec3(0, -0.9, 0.0), vec3(2, 1.4 + 0.0006*sin(7*p.x + 5*p.y + 5*p.z), 50000)), floorType };
   
    
    vec3 p3 = p - path(-5.5);
    p3.xy *= rot(sin(iTime)*0.2);
    p3.xz *= rot(cos(iTime)*0.5);
    DistanceInfo pirate = pirateyText(p3);
    DistanceInfo wireBox = {sdBoxFrame(p3 - vec3(0, 0, 0), vec3(0.6, 0.3, 0.), 0.005), textType};
	pirate = un(wireBox, pirate);
    DistanceInfo res = sunk(cylinder, floorBox, 0.3);
    res = sunk(res, pirate, 0.05);
    return res;
}


)""
