R""(
#define CUSTOM_VELOCITY 10.0
#include "edison2021/tunnel_base.fs"

const int cubeType = lastBaseType + 1;


DistanceInfo wireCube(vec3 p) {
    float f = 2*PI/130.0 * 22;


    vec3 q = p;
    q.xz *= rot(sin(f*3*iAbsoluteTime)*cos(f*5*iAbsoluteTime));

    mo(q.xy, vec2(0.1, 0.3));
    q.x-=0.2;
    q.zy *= rot(iAbsoluteTime);

    mo(q.xy, vec2(0.4 + 0.2*sin(2*f*iAbsoluteTime), 0.09+ 0.2*cos(f*5*iAbsoluteTime)));
    q *= 1.0 + 0.3*sin(f*15*iAbsoluteTime)*cos(f*3*iAbsoluteTime);
    
    float dFrame = sdBoxFrame(q, vec3(0.3), 0.01);

    DistanceInfo wireCube = {dFrame, cubeType};
    return wireCube;
}


VolumetricResult evaluateLight(in vec3 p)
{
    float s = 1.5;
    DistanceInfo cube = wireCube(s*(p - path(-7.2)));
    cube.distance /= s;

    float d = 1.0;

	float d2 = max(0.001, cube.distance);
    
	float strength = 0.0 + 1.0 * smoothstep(11.0, 16.0, iTime);
    vec3 col = vec3(0.1, 0.1, 1.0);
	vec3 res2 = col * strength / (d2 * d2 );
    

	return VolumetricResult(mix(d, d2, smoothstep(10.0, 13.0, iTime)), res2);
}

float getFogAmount(in vec3 p) 
{
    return 0.0005;
}

float getReflectiveIndex(int type)
{
    if (type == wallType)
        return 0.05;
    if (type == floorType)
        return 0.05;
    return 0.1;
}


vec3 getAmbientColor(int type, vec3 pos) 
{
    if (type == wallType || type == floorType){
        vec3 wall = 0.9*vec3(0.2, 0.2, 0.2); 
        vec3 wallColored = 3.0*palette(0.05*pos.z, vec3(0.5), vec3(0.5), vec3(1.0, 1.0, 1.0), vec3(0.0, 0.33, 0.67));
        wall = mix(wall, wallColored, smoothstep(1.0, 8.0, iTime));
        vec3 wallColored2 = 0.6*vec3(0.2, 0.2, 3.0);
        wall = mix(wall, wallColored2, smoothstep(1.0, 8.0, iTime));
        return wall;
    } else if (type == cubeType) {
        return 30*vec3(1.0, 1.0, 0.1);
	}
    return vec3(0.0);
}


DistanceInfo map(in vec3 p)
{
   // p.y+=0.5 * ( smoothstep(1.0, 8.0, iTime));
    DistanceInfo cylinder = {-sdCappedCylinder(p.xzy - tunnelDelta(p.z) - vec3(0, 0., 0.0), vec2(1 + 0.1*filteredLines(10*atan(p.y), 1.1), 50000)), wallType };
    DistanceInfo floorBox = {-sdBox(p - tunnelDelta(p.z) + vec3(0, -0.8 + 0.5 * ( smoothstep(1.0, 8.0, iTime)), 0.0), vec3(2, 1.4 + 0.0006*sin(7*p.x + 5*p.y + 5*p.z), 50000)), floorType };

  //  DistanceInfo cylinder = {-sdCappedCylinder(p.xzy - tunnelDelta(p.z) - vec3(0, 0., 0.0), vec2(1 + 0.1*filteredLines(10*atan(p.y), 1.1), 50000)), wallType };
  //  DistanceInfo floorBox = {-sdRoundBox(p - tunnelDelta(p.z) + vec3(0, -1.3 * (1.0 - smoothstep(1.0, 6.0, iTime)), 0.0), vec3(2.0 + 0.006*sin(7*p.x + 5*p.y + 5*p.z), 1.4 + 0.006*sin(7*p.x + 5*p.y + 5*p.z), 50000), 0.5), wallType };
    cylinder.distance = mix(cylinder.distance, floorBox.distance, smoothstep(1.0, 8.0, iTime) - smoothstep(17.0, 21.0, iTime));
    floorBox.distance = mix(floorBox.distance, cylinder.distance, smoothstep(18.0, 22.0, iTime));
    DistanceInfo tunnel = sunk(cylinder, floorBox, 0.3*(1.0 - smoothstep(13.0, 15.0, iTime)));


    DistanceInfo res = tunnel;//un(cube, tunnel);
    return res;
}

)""
