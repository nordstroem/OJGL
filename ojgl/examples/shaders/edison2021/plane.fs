R""(
#define CUSTOM_DELTA_MODIFIER
float tunnelDeltaModifier() {
    return 0.0;
};

#define CUSTOM_CAMERA 1

#include "edison2021/tunnel_base.fs"

const int pipesAType = lastBaseType + 1;
const int pipesBType = lastBaseType + 2;
const int starType = lastBaseType + 3;
const int planeType = lastBaseType + 4;

const float transitionStartTime = 10;

float smoothstepIntegral(float t, in float T )
{
    if( t>=T ) return t - 0.5*T;
    float f = t/T;
    return f*f*f*(T-t*0.5);
}


vec3 path2(float zPos) {
    float period = 200 * PI; // (depends on tunnelDelta)
    vec3 pos = vec3(0.0, 0.0, mod(zPos, period));
    pos += tunnelDelta(pos.z);
    return pos;
}

void updateCamera(inout vec3 rayOrigin, inout vec3 lookAt) {
    const float lol = 29 + 26 + 25 + 31;
    if (iTime > 15.0) {
        float t0 = iTime - 15.0;
        float orgZ = iAbsoluteTime * 10.0;

        float newZ = (lol + 15) * 10.0 + 10.0 * (t0 - smoothstepIntegral(t0, 20.0));

        // iAbsoluteTime * 10.0
        rayOrigin = path2(-newZ);
        lookAt = path2(-newZ-10.5);
    }
}

float birdDis(in vec3 p) {
    const float since = iTime - transitionStartTime;
    p.y += smoothstep(0.0, 0.5, since) * 0.3;


    float birdPos = -5.5;
    if (iTime > 15) {
        //birdPos -= iTime - 15;
    }
    p = p - path(birdPos);
    p.y -= 0.1*sin(iTime);
    p.zy *= rot(-0.15);

    p = p.xzy;
    //p.y += 0.5*cos(3*iTime)*sin(iTime);

    DistanceInfo b0 = {sdVerticalCapsule(p - vec3(0, 0, -0.035), 0.3, 0.02), sphereType};

    p.x = abs(p.x);

    float rotFrequency = 7;
    float animationLength = 10;
    float t = mod(iTime-4.0, animationLength);
    float animation = 0.1 + smoothstep(0, 1, t) - smoothstep(6.0, 7.0, t);
    float rotAmplitude = 0.5 * animation;
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

   return bird.distance;
}

vec3 planeFlyIn(in vec3 p) {
     p.z -= 7 * (1.0 - smoothstep(0, 5, iTime));
     return p;
}

VolumetricResult evaluateLight(in vec3 p)
{
    DistanceInfo res = {99999, 0};

    {
        {
            p = planeFlyIn(p);

            float r = 0.06 + sin(p.z* 100) * 0.05;
            r -= (p.z - path(-5.5).z) * 0.08;
            float d3 = sdCappedCylinder((p - path(-5.5) - vec3(0.5, -0.6, 0.5)).xzy, vec2(r, 0.3));
            float d4 = sdCappedCylinder((p - path(-5.5) - vec3(-0.5, -0.6, 0.5)).xzy, vec2(r, 0.3));
            float d = min(d3, d4);
            res = un(res, DistanceInfo(d, planeType));
        }

        if (iTime > transitionStartTime) {

            float dd = birdDis(p);
            res.distance = mix(res.distance, dd, min(1.0, (iTime - transitionStartTime) * 0.8));
        }

    }

    float d = max(0.001, res.distance);
    vec3 col = vec3(0);
	float strength = 10;
    if (res.type == starType) {
        col = vec3(0.1, 0.1, 1.0);
        strength = 100;
    } else if (res.type == pipesAType) {
        col = vec3(1.0, 0.5, 0.1);
    } else if (res.type == pipesBType) {
        col = vec3(1.0, 1.0, 1.0);
    } else if (res.type == planeType) {
        col = vec3(1.0, 0.1, 0.01);
        vec3 col2 = vec3(1.0, 0.0, 0.01);


        //col = mix(col, col2, mod(p.z * 100, 1.0)* mod(p.z * 100, 1.0)* mod(p.z * 100, 1.0));
        col = mix(col, col2, mod(p.z, 1.0));


        strength = 10 + sin(iTime * 30) * 1;
    }

    col *=  smoothstep(2, 3, iTime);
	vec3 res2 = col * strength / (d * d);
	return VolumetricResult(d, res2);
}

float getFogAmount(in vec3 p)
{
    return 0.0005 + (1.0 - smoothstep(0, 2, iTime));
}

float getReflectiveIndex(int type)
{
    if (type == wallType)
        return 0.02;
    if (type == pipesAType || type == pipesBType)
        return 0.8;
    if (type == starType)
        return 0.8;
    if (type == planeType)
        return 0.8;
    return 0.0;
}

vec3 getAmbientColor(int type, vec3 pos)
{
    vec3 wall = 0.5*vec3(0.2, 0.2, 0.2);
    if (type == wallType || type == floorType){
        return wall;
    }
    if (type == pipesAType) {
        return vec3(1, 0, 1);
    }
    if (type == pipesBType) {
        return vec3(0, 1, 1);
    }
    if (type == starType) {
        return 30 * vec3(1, 1, 1);
    }
    if (type == planeType) {
        return vec3(1, 1, 1);
    }
    return vec3(0.1);
}

DistanceInfo map(in vec3 p)
{
    vec3 p2 = p.xzy - tunnelDelta(p.z);
    DistanceInfo cylinder = {-sdCappedCylinder(p.xzy - tunnelDelta(p.z) - vec3(0, 0, 0.02 * mod(p.z * 6.0, 1.0)), vec2(1 + 0.0*filteredLines(10*atan(p.y), 1.1) + 0.1*filteredLines(5*p2.x, 1.1), 50000)), wallType };
    DistanceInfo floorBox = {-sdBox(p - tunnelDelta(p.z) + vec3(0, -1.2, 0.0), vec3(3, 2.0 + 0.0006*sin(7*p.x + 5*p.y + 5*p.z), 50000)), floorType };

	DistanceInfo res =  cylinder;



    {
       p = planeFlyIn(p);

        if (iTime > transitionStartTime + 0.5) {
            const float since = iTime - transitionStartTime - 0.5;
            p.y += 0.2 * since;
            p.z -= since * 2.0;
        }
        float d = sdCappedCylinder((p - path(-5.5) - vec3(0, -0.5, 0.0)).xzy, vec2(0.12, 0.4));
        float d3 = sdCappedCylinder((p - path(-5.5) - vec3(0.5, -0.6, 0.0)).xzy, vec2(0.08, 0.2));
        float d4 = sdCappedCylinder((p - path(-5.5) - vec3(-0.5, -0.6, 0.0)).xzy, vec2(0.08, 0.2));

        float d5 = sdRoundBox(p - path(-5.5) - vec3(0, -0.3, 0.3), vec3(0.01, 0.1, 0.1), 0.01);

        float d2 = sdRoundBox(p - path(-5.5) - vec3(0, -0.5, 0.0), vec3(0.8, 0.01, 0.1), 0.01);
        d = smink(d, d2, 0.1);
        d = smink(d, d3, 0.1);
        d = smink(d, d4, 0.1);
        d = smink(d, d5, 0.1);
        res = un(res, DistanceInfo(d, planeType));
    }

    return res;
}


)""
