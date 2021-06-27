R""(
#include "edison2021/tunnel_base.fs"

VolumetricResult evaluateLight(in vec3 p)
{
    p = p - path(-5.5);
    p.y -= 0.3*sin(iTime);
    p.zy *= rot(-0.15);
    
    p = p.xzy;
    p.y += cos(3*iTime)*sin(iTime);

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

    float d = bird.distance;

	d = max(0.001, d);
    
	float strength = 5;
    vec3 red = vec3(1.0, 0.0, 1.0);
    vec3 yellow = vec3(1.0, 1.0, 0.0);
	vec3 col = mix(red, yellow, 0.01);
	vec3 res2 = col * strength / (d * d);
	return VolumetricResult(d, res2);
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
    vec3 wall = 0.5*vec3(0.2, 0.2, 0.2); 
    if (type == wallType || type == floorType){
        return wall;
    }
    return vec3(0.1);
}

DistanceInfo map(in vec3 p)
{
    vec3 p2 = p.xzy - tunnelDelta(p.z);
    DistanceInfo cylinder = {-sdCappedCylinder(p.xzy - tunnelDelta(p.z), vec2(1.0 + 0.1*filteredLines(10*atan(p.y), 1.1), 50000)), wallType };
    DistanceInfo floorBox = {-sdBox(p - tunnelDelta(p.z) + vec3(0, -0., 0.0), vec3(0.8, + 0.8 + 0.0006*sin(7*p.x + 5*p.y + 5*p.z), 50000)), wallType};
    DistanceInfo tunnel = sunk(cylinder, floorBox, 0.1);
    return cylinder;
}


)""
