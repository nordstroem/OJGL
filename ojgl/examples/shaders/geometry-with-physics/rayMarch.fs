R""(
#version 430
#include "shaders/common/noise.fs"
#include "shaders/common/primitives.fs"
#include "shaders/common/raymarch_utils.fs"
#include "shaders/common/utils.fs"

in vec2 fragCoord;
out vec4 fragColor;

uniform float iTime;
uniform vec2 iResolution;
uniform mat4 iCameraMatrix;

uniform sampler2D inTexture0;
uniform sampler2D inTexture1;

const int sphereType = 1;
const int wallType = 2;

DistanceInfo map(in vec3 p)
{
	float bt;
	float ft = modf(iTime, bt);

	vec3 bp = p;
	vec3 wp = p;
	vec3 lp = p;
	vec3 lp2 = p;

	p.xy *= rot(0.20);
	float scale = 0.6;
	float increase = scale*pow(clamp(p.x, 0.0, 0.66)*0.98, 6);
	float cone = sdCappedCylinder(p.zxy, vec2((0.2 + increase)*scale  , 0.7));
	float cone3 = sdCappedCylinder(p.zxy - vec3(0.0,0.4,0), vec2(0.215*scale, 0.01));
	cone3 = min(cone3, sdCappedCylinder(p.zxy - vec3(0.0,-0.3,0), vec2(0.205*scale, 0.01)));
	cone = min(cone, cone3);
	cone += + 0.001*noise_3(p*150.);

	wp.z = abs(wp.z);
	float wheel  = sdCappedCylinder(wp.xzy + vec3(0.35, -0.25, 0.2), vec2(0.5, 0.04));
	float wheel2 = sdCappedCylinder(wp.xzy + vec3(0.35, -0.25, 0.2), vec2(0.47, 0.2));
	wheel = max(wheel, -wheel2);
	
	bp.z = abs(bp.z);
	bp.x += 0.35;
	bp.z += -0.25;
	bp.y += 0.2;
	bp.xy *= rot(-0.2*iTime);

	pModPolar(bp.xy, 9);

	float bar = sdBox(bp.xzy, vec3(0.47, 0.015, 0.015));
	wheel = min(wheel, bar);

	float cone2 = sdCappedCylinder(p.zxy + vec3(0.0,-0.1,0), vec2(0.15 * scale, 0.7));
	cone = max(cone, -cone2);

	lp.z = abs(lp.z);
	lp.x += 0.14;
	lp.y += 0.1;
	lp.z += -0.15;
	float lower = sdBox(lp, vec3(0.2, 0.1, 0.05));

	lp2.z = abs(lp2.z);
	lp2.z -= 0.15;
	lp2.x += 1.2;
	lp2.y += 0.4;
	lp2.xy *= rot(0.3);
	float decrease = lp.x * 0.04;
	float lower2 = sdBox(lp2, vec3(1.0, 0.1 + decrease, 0.05));

	float d = min(min(lower, lower2), min(cone, wheel));
	DistanceInfo sphere1 = {d, sphereType };
    return sphere1;
}

float getReflectiveIndex(int type)
{
    return type == wallType ? 0.6 : 0.0;
}

vec3 getMarchColor(in MarchResult result)
{
    if (result.type != invalidType) {
		if (result.type == wallType)
			return 0.3*vec3(1.0);
        vec3 ambient = vec3(0.1, 0.1, 0.1);
        //vec3 invLight = -normalize(vec3(-0.7, -0.2, -0.5));
		vec3 invLight = -normalize(vec3(0.5, -0.7, -0.3) - result.position);
        vec3 normal = normal(result.position);
        float diffuse = max(0., dot(invLight, normal));
        return vec3(ambient + 0.6*diffuse - result.steps / 100.f);
    } else {
        return vec3(0.3);
    }
}

vec3 getMeshColor(in vec3 pos, in vec3 normal)
{
    vec3 ambient = 1.3*vec3(0.8, 0.3, 0.1);
	vec3 invLight = -normalize(vec3(0,1,-3) - pos);
    float diffuse = max(0., dot(invLight, normal));
    return vec3(ambient * (0.1 + diffuse));
}

void main()
{
    float u = (fragCoord.x - 0.5) * iResolution.x / iResolution.y;
    float v = (fragCoord.y - 0.5);
    vec3 rayOrigin = (iCameraMatrix * vec4(u, v, -1.0, 1.0)).xyz;
    vec3 eye = (iCameraMatrix * vec4(0.0, 0.0, 0.0, 1.0)).xyz;
    vec3 rayDirection = normalize(rayOrigin - eye);

    MarchResult result = march(eye, rayDirection);
    vec3 color = getMarchColor(result);
	
    float reflectiveIndex = getReflectiveIndex(result.type);
    if (reflectiveIndex > 100.0) {
        rayDirection = reflect(rayDirection, normal(result.position));
        MarchResult reflectResult = march(result.position + 0.1 * rayDirection, rayDirection);
        vec3 newColor = getMarchColor(reflectResult);
        color = mix(color, newColor, reflectiveIndex);
    }


	if (abs(texture(inTexture0, fragCoord.xy).w - 0.5) < 0.01) {
		vec3 meshPos = texture(inTexture0, fragCoord.xy).xyz;
		
		if (length(eye - meshPos) < length(eye - result.position)) {
			vec3 meshNormal = texture(inTexture1, fragCoord.xy).xyz;

			float mixFactor = result.type == sphereType ? 0.1 : 1.0;
			color = mix(color, getMeshColor(meshPos, meshNormal), mixFactor);
		}
	}
	
	fragColor = vec4(color, 1.0);
}

)""
