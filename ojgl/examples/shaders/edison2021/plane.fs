R""(
#include "edison2021/tunnel_base.fs"

const int pipesAType = lastBaseType + 1;
const int pipesBType = lastBaseType + 2;
const int starType = lastBaseType + 3;
const int planeType = lastBaseType + 4;
const int lightTpe = lastBaseType + 5;


VolumetricResult evaluateLight(in vec3 p)
{
 
    

    DistanceInfo res = {99999, 0};

    {
        float r = 0.06 + sin(p.z* 100) * 0.05;
        r -= (p.z - path(-5.5).z) * 0.08;
        float d3 = sdCappedCylinder((p - path(-5.5) - vec3(0.5, -0.6, 0.5)).xzy, vec2(r, 0.3));
        float d4 = sdCappedCylinder((p - path(-5.5) - vec3(-0.5, -0.6, 0.5)).xzy, vec2(r, 0.3));
        float d = min(d3, d4);
        res = un(res, DistanceInfo(d, planeType));
    }

    {
        float d = length(p - path(-5.5)) - 0.01;
        //res = un(res, DistanceInfo(d, lightTpe));
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
    } else if (res.type == lightTpe) {
        col = vec3(1.0, 0.01, 0.01);
        strength = 5;
    }

	vec3 res2 = col * strength / (d * d);
	return VolumetricResult(d, res2);
}

float getFogAmount(in vec3 p) 
{
    return 0.0005;
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
