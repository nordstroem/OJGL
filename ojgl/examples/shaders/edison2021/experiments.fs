R""(
#include "edison2021/tunnel_base.fs"

DistanceInfo birdie(in vec3 p) {

 p = p - path(-5.5);
    p.y -= 0.1*sin(iTime);
    p.zy *= rot(-0.15);
    
    p = p.xzy;
    p.y += 0.5*cos(3*iTime)*sin(iTime);

    DistanceInfo b0 = {sdVerticalCapsule(p - vec3(0, 0, -0.035), 0.3, 0.02), sphereType};

    p.x = abs(p.x);
    
    float rotFrequency = 7;
    float animationLength = 15;
    float t = mod(iTime, animationLength);
    float animation = 0.1 + smoothstep(0, 1, t) - smoothstep(9.0, 10.0, t);
    float rotAmplitude = 0.4 * animation;
    p.xz *= rot(rotAmplitude*sin(rotFrequency*iTime));

    DistanceInfo b1 = {sdBox(p, vec3(0.4, 0.2, 0.005)), sphereType};
    

    p = p - vec3(0.6, 0, 0);
    p.x+=0.2;
    p.xz *= rot(rotAmplitude*1.1*sin(rotFrequency*(iTime - 0.15)));
    p.x-=0.2;

    float tipWidth = 0.2;
    float r = clamp(1-(p.x+tipWidth)/(tipWidth*2), 0, 1);

    DistanceInfo b2 = {sdBox(p, vec3(tipWidth, 0.18*r, 0.001)), sphereType};
    DistanceInfo bird = sunk(b0, sunk(b1, b2, 0), 0.15);

    return bird;
}

VolumetricResult evaluateLight(in vec3 p)
{
    DistanceInfo bird = birdie(p);
    float d = bird.distance;

	d = max(0.001, d);
    
	float strength = 5;
    if (bird.type == wallType)
        strength = 0.03;
    vec3 red = vec3(1.0, 0.0, 0.0);
    vec3 yellow = vec3(1.0, 1.0, 0.0);
	vec3 col = mix(red, yellow, 0.01);
	vec3 res2 = col * strength / (d * d);
	return VolumetricResult(d, res2);
}

float getFogAmount(in vec3 p) 
{
    return 0.0001;
}

DistanceInfo map(in vec3 p)
{
    vec3 p2 = p.xzy - tunnelDelta(p.z);
    DistanceInfo cylinder = {-sdCappedCylinder(p.xzy - tunnelDelta(p.z) - vec3(0, 0., 0.0), vec2(1 + 0.1*filteredLines(10*atan(p.y), 1.1), 50000)), wallType };
    DistanceInfo floorBox = {-sdBox(p - tunnelDelta(p.z) + vec3(0, -0.9, 0.0), vec3(2, 1.4 + 0.0006*sin(7*p.x + 5*p.y + 5*p.z), 50000)), floorType };
   
    
    DistanceInfo pirate = pirateyText(p - path(-5.5) - vec3(0, -0.3, 0));
	DistanceInfo res = sunk(cylinder, floorBox, 0.3);
    res = un(res, pirate);
    return res;
}


)""
