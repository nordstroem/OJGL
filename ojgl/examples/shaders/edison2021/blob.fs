R""(
#define CUSTOM_CAMERA
#include "edison2021/tunnel_base.fs"

uniform float totalHits;

const int blobType = lastBaseType + 1;
const int headType = lastBaseType + 2;

VolumetricResult evaluateLight(in vec3 p)
{
    vec3 orgP = p;
    p = orgP.xzy - vec3(0.0, path(-5.5).z, 0.0);


    float wScaler0 = 1.0 - smoothstep(22.0, 24.0, iTime);
    float wScaler = smoothstep(0.0, 10.0, iTime);
    float wScaler2 = smoothstep(5.0, 10.0, iTime);
    float wScaler3 = smoothstep(9.0, 10.0, iTime);

    float wormRadius = 3.0 * wScaler;
    float r = wormRadius - (p.y + wormRadius )/2;
    float rr = (2*PI/130) * 200;
    float yDelta = -0.9 - -0.15*r + 0.5*psin(5 * r + rr * iTime)*wScaler0 - (1-wScaler0);

    vec3 p2 = orgP - path(-5.5 -wormRadius-0.05) - vec3(-wScaler2*0.2, yDelta+wScaler2*0.08, 0.0) + 0.1*sin(3*p.x);
    DistanceInfo head = {sdSphere(p2, 0.07*wScaler2), headType};

    p2 = orgP - path(-5.5 -wormRadius-0.05) - vec3(wScaler*0.2, yDelta+wScaler2*0.08, 0.0) + 0.1*sin(3*p.x);
    DistanceInfo head2 = {sdSphere(p2, 0.07*wScaler2), headType};
    head = un(head, head2);



    p2 = orgP - path(-5.5 -wormRadius-0.05) - vec3(0.0, yDelta, -0.0) + 0.1*sin(3*p.x);
    p2.y*=-1;
    DistanceInfo head3 = {sdSphere(p2 - vec3(0.0, 0.2, 0.0), wScaler3*(0.04)), headType};
    head = un(head, head3);

    p = orgP.xzy - vec3(0.0, path(-5.5).z, 0.0);
    float cylinderRadius = 0.03 + 0.04*r*r;
    //cylinderRadius *= 0.3;
    DistanceInfo blob = {sdCappedCylinder(p - tunnelDelta(orgP.z) + 0.1*sin(3*p.x) - vec3(0, 0, yDelta), vec2(cylinderRadius, wormRadius)), blobType};

    head = un(head, blob);

    float d = max(0.0001, head.distance);

	float strength = head.type == headType && iTime > 12.0 && iTime < 20.0 ? 2.0 : wScaler*0.0006;
    //strength *= 10000;
	//vec3 col = vec3(0.1, 1.0, 0.1);
	vec3 col = vec3(0.5 + 0.5 * sin(totalHits + 2.0), 0.5 + 0.5 * cos(totalHits * 2.0), 0.5 + 0.5 * sin(totalHits * 3.0));
	col = normalize(col);
    vec3 res2 = col * strength / (d * d);
	return VolumetricResult(d, res2);
}

float getFogAmount(in vec3 p)
{
    return 0.00002;
}

float getReflectiveIndex(int type)
{
    if (type == wallType)
        return 0.05;
    if (type == floorType)
        return 0.05;
    if (type == blobType)
        return 0.0;
    if (type == headType)
        return 0.0;
    return 0.0;
}

vec3 getAmbientColor(int type, vec3 pos)
{
    vec3 wall = 0.9*vec3(0.2, 0.2, 0.2);
    if (type == wallType || type == floorType){
        return wall;
    }
    if (type == blobType) {
        return 15*vec3(0.3, 0.5, 0.2);
	}
    if (type == headType) {
        return 0.0*vec3(1.0, 1.0, 1.0);
	}
    return vec3(0.1);
}



DistanceInfo map(in vec3 p)
{
    DistanceInfo cylinder = {-sdCappedCylinder(p.xzy - tunnelDelta(p.z) - vec3(0, 0., 0.0), vec2(1 + 0.1*filteredLines(10*atan(p.y), 1.1), 50000)), wallType };
    DistanceInfo floorBox = {-sdBox(p - tunnelDelta(p.z) + vec3(0, -0.8, 0.0), vec3(2, 1.4 + 0.0006*sin(7*p.x + 5*p.y + 5*p.z), 50000)), floorType };
    DistanceInfo tunnel = sunk(cylinder, floorBox, 0.3);



    return tunnel;//sunk(tunnel, blob, 0.1);
}


void updateCamera(inout vec3 rayOrigin, inout vec3 lookAt)
{
    if (iTime > 12.0 && iTime < 20.0)
        rayOrigin = path(-22.0) - vec3(0.0, 0.0, 0.0);
    else
        rayOrigin = path(0.0);

}

)""
