R""(
#include "edison2021/tunnel_base.fs"

uniform sampler2D moltresTexture;
uniform sampler2D ojTexture;
uniform sampler2D edisonTexture;

const int moltresType = lastBaseType + 1;
const int ojType = lastBaseType + 2;
const int edisonType = lastBaseType + 3;

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

DistanceInfo moltresText(in vec3 p)
{
    vec2 uv;
    DistanceInfo box = {uvBox(p, vec3(0.5, 0.2, 0.0), uv), moltresType };
    uv.x *=-1;
    if (true && box.distance < S_distanceEpsilon) {
        float s = texture(moltresTexture, uv).x;
        if (s > 0.2) { // If not on text
            box.type = moltresType;
            box.distance = 5000000;
        }
	}
        
    return box;
}

DistanceInfo byOJText(in vec3 p)
{
    vec2 uv;
    DistanceInfo box = {uvBox(p, vec3(0.3, 0.2, 0.0), uv), ojType };
    uv.x *=-1;
    if (true && box.distance < S_distanceEpsilon) {
        float s = texture(ojTexture, uv).x;
        if (s > 0.2) { // If not on text
            box.type = ojType;
            box.distance = 5000000;
        }
	}
    return box;
}


DistanceInfo edisonText(in vec3 p)
{
    vec2 uv;
    DistanceInfo box = {uvBox(p, vec3(0.8, 0.2, 0.0), uv), edisonType };
    uv.x *=-1;
    if (true && box.distance < S_distanceEpsilon) {
        float s = texture(edisonTexture, uv).x;
        if (s > 0.2) { // If not on text
            box.type = edisonType;
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
    return 0.1;
}

vec3 getAmbientColor(int type, vec3 pos) 
{
    vec3 wall = 0.9*vec3(0.2, 0.2, 0.2); 
    if (type == wallType || type == floorType){
        return wall;
    }
    if (type == moltresType) {
        return 50*vec3(1.0, 0.1, 0.0);
	}
     if (type == ojType) {
        return 50*vec3(1.0, 1.0, 0.05);
	}
    if (type == edisonType) {
        return 50*palette(sin(pos.x*0.2) + 0.2*iTime, vec3(0.5), vec3(0.5), vec3(1.0, 0.7, 0.4), vec3(0.0, 0.15, 0.2));
	}
    return vec3(0.1);
}

DistanceInfo map(in vec3 p)
{
    vec3 p2 = p.xzy - tunnelDelta(p.z);
    DistanceInfo cylinder = {-sdCappedCylinder(p.xzy - tunnelDelta(p.z) - vec3(0, 0., 0.0), vec2(1 + 0.1*filteredLines(10*atan(p.y), 1.1), 50000)), wallType };
    DistanceInfo floorBox = {-sdBox(p - tunnelDelta(p.z) + vec3(0, -0.9, 0.0), vec3(2, 1.4 + 0.0006*sin(7*p.x + 5*p.y + 5*p.z), 50000)), floorType };
   
    
    float z = -6.5 + 40*smoothstep(5.0, 10.0, iTime) + 30*smoothstep(15., 20.0, iTime) + 40*smoothstep(25., 30.0, iTime);
    vec3 p3 = p - path(z);
    p3.xy *= rot(sin(iTime)*0.2);
    p3.xz *= rot(cos(iTime)*0.5);

    DistanceInfo moltres = moltresText(p3);
    DistanceInfo wireBox = {sdBoxFrame(p3 - vec3(0, 0, 0), vec3(0.6, 0.3, 0.0), 0.005), moltresType};
    moltres = un(moltres, wireBox);

    p3 = p - path(z - 40);
    p3.xy *= rot(sin(iTime)*0.2);
    p3.xz *= rot(cos(iTime)*0.5);

    DistanceInfo byOJ = byOJText(p3);
    p3.xy *= rot(iTime);
    p3.xz *= rot(iTime*3);
    DistanceInfo wireBox2 = {sdBoxFrame(p3 - vec3(0, 0, 0), vec3(0.4, 0.4, 0.4), 0.005), ojType};
    byOJ = un(byOJ, wireBox2);


    p3 = p - path(z - 70);
    p3.xy *= rot(sin(iTime)*0.2);
    p3.xz *= rot(cos(iTime)*0.5);

    DistanceInfo edison = edisonText(p3);
    DistanceInfo wireBox3 = {sdBoxFrame(p3 - vec3(0, 0, 0), vec3(0.9, 0.3, 0.0), 0.005), edisonType};
    edison = un(edison, wireBox3);


	DistanceInfo text = un(edison, un(moltres, byOJ));

    DistanceInfo res = sunk(cylinder, floorBox, 0.3);
    res = sunk(res, text, 0.05);
    return res;
}

)""
