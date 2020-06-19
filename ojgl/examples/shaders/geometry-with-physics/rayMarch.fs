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
uniform sampler2D inTexture2;
uniform sampler2D inTexture3;

const int sphereType = 1;
const int wallType = 2;
const int groundType = 3;

DistanceInfo cannon(in vec3 p)
{
    float bt;
	float ft = mod(iTime, 2.0);

	float recoilLength = 0.2; 
	float recoil = recoilLength * smoothstep(0.0, 0.1, ft) * (1.f - smoothstep(0.1, 2.0, ft));
	p.x += recoil;

	vec3 bp = p;
	vec3 wp = p;
	vec3 lp = p;
	vec3 lp2 = p;

	p.xy *= rot(0.20 + 0.6*recoil);
	float scale = 0.6;
	float increase = scale*pow(clamp(p.x, 0.0, 0.66)*0.98, 6);
	float cone = sdCappedCylinder(p.zxy, vec2((0.2 + increase)*scale  , 0.7));
	float cone3 = sdCappedCylinder(p.zxy - vec3(0.0,0.4,0), vec2(0.215*scale, 0.01));
	cone3 = min(cone3, sdCappedCylinder(p.zxy - vec3(0.0,-0.3,0), vec2(0.205*scale, 0.01)));
	cone3 = min(cone3, sdCappedCylinder(p.zxy - vec3(0.0,-0.7,0), vec2(0.205*scale, 0.015)));
	cone = min(cone, cone3);
	cone += 0.001*noise_3(p*150.);

	wp.z = abs(wp.z);
	const float wheelSize = 0.5;
	float wheel  = sdCappedCylinder(wp.xzy + vec3(0.35, -0.3, 0.2), vec2(wheelSize, 0.04));
	float wheel2 = sdCappedCylinder(wp.xzy + vec3(0.35, -0.3, 0.2), vec2(wheelSize-0.03, 0.2));
	wheel = max(wheel, -wheel2);
	
	bp.z = abs(bp.z);
	bp.x += 0.35;
	bp.z += -0.3;
	bp.y += 0.2;
	bp.xy *= rot(recoil / wheelSize);

	pModPolar(bp.xy, 9);

	float bar = sdBox(bp.xzy, vec3(wheelSize-0.03, 0.015, 0.015));
	wheel = min(wheel, bar);

	float cone2 = sdCappedCylinder(p.zxy + vec3(0.0,-0.1,0), vec2(0.15 * scale, 0.7));
	cone = max(cone, -cone2);

	lp.z = abs(lp.z);
	lp.x += 0.14;
	lp.y += 0.1;
	lp.z += -0.15;
	float lower = sdBox(lp, vec3(0.2, 0.1, 0.1));

	lp2.z = abs(lp2.z);
	lp2.z -= 0.2;
	lp2.x += 1.2;
	lp2.y += 0.4;
	lp2.xy *= rot(0.3);
	float decrease = lp.x * 0.04;
	float lower2 = sdBox(lp2, vec3(1.0, 0.1 + decrease , 0.05));

	float d = min(min(lower, lower2), min(cone, wheel));
	DistanceInfo sphere1 = {d, sphereType };
    return sphere1;
}

DistanceInfo ground(in vec3 p)
{
	p.y += 0.92;
	float d = sdRoundBox(p, vec3(3.0, 0.2, 3.0), 0.02);	
	DistanceInfo plane = {d, groundType};
	return plane;
}

DistanceInfo map(in vec3 p)
{
	return un(ground(p), cannon(p));
}

float getReflectiveIndex(int type)
{
    return type == wallType ? 0.6 : 0.0;
}

vec4 genAmbientOcclusion(vec3 ro, vec3 rd)
{
    vec4 totao = vec4(0.0);
    float sca = 1.0;

    for (int aoi = 0; aoi < 5; aoi++)
    {
        float hr = 0.01 + 0.02 * float(aoi * aoi);
        vec3 aopos = ro + rd * hr;
        float dd = map(aopos).distance;
        float ao = clamp(-(dd - hr), 0.0, 1.0);
        totao += ao * sca * vec4(1.0, 1.0, 1.0, 1.0);
        sca *= 0.75;
    }

    const float aoCoef = 0.5;
    totao.w = 1.0 - clamp(aoCoef * totao.w, 0.0, 1.0);

    return totao;
}

vec3 getMarchColor(in MarchResult result, in vec3 rayDirection)
{
	vec3 invLight = -normalize(vec3(0.5, -0.7, -0.3));
	vec3 normal = normal(result.position);
	float diffuse = max(0., dot(invLight, normal));
	float ao = genAmbientOcclusion(result.position, rayDirection).x;

    if (result.type != invalidType) {
		if (result.type == wallType)
			return 0.3*vec3(1.0);
		else if (result.type == groundType) {
			return vec3(diffuse*ao);
		} else {
		    vec3 ambient = vec3(0.1, 0.1, 0.1);
			//vec3 invLight = -normalize(vec3(-0.7, -0.2, -0.5));
			//return vec3(ambient + 0.6*diffuse - result.steps / 100.f);
			return vec3(ambient + 0.6*diffuse - result.steps / 100.f);
		}
    } else {
		float tau = 2. * PI;
		float phi = 0.5 + atan(rayDirection.x, rayDirection.z) / tau - 0;
		float theta = 0.5 - asin(rayDirection.y) / PI;
	    float u2 = cos(2*tau*sin(3*tau*phi)) + cos(0.3 + 2.*tau*phi) + sin(5.*theta);

//        return 0.2*vec3(0.1, 0.2, 0.4*(0.5 + 0.5*u2));

		u2 = 0.8*fbm3_high(10.*vec3(psin(tau * phi), psin(tau * theta), 0.04*iTime), 0.85, 2.2);
        return 0.08*vec3(u2, 0.5, 0.5);

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
    vec3 color = getMarchColor(result, rayDirection);
	
    float reflectiveIndex = getReflectiveIndex(result.type);
    if (reflectiveIndex > 100.0) {
        rayDirection = reflect(rayDirection, normal(result.position));
        MarchResult reflectResult = march(result.position + 0.1 * rayDirection, rayDirection);
        vec3 newColor = getMarchColor(reflectResult, rayDirection);
        color = mix(color, newColor, reflectiveIndex);
    }


	if (abs(texture(inTexture0, fragCoord.xy).w - 1.0) < 0.01) {
		vec3 meshPos = texture(inTexture0, fragCoord.xy).xyz;
		
		if (result.type == invalidType || length(eye - meshPos) < length(eye - result.position)) {
			vec3 meshNormal = texture(inTexture1, fragCoord.xy).xyz;
			//float alpha = texture(inTexture1, fragCoord.xy).w;
			float mixFactor = result.type == sphereType ? 1.0 : 1.0;
			color = mix(color, vec3(1.0), meshNormal.x);
		}
	}


	if (abs(texture(inTexture2, fragCoord.xy).w - 1.0) < 0.01) {
		vec3 meshPos = texture(inTexture2, fragCoord.xy).xyz;
		
		if (result.type == invalidType || length(eye - meshPos) < length(eye - result.position)) {
			vec3 meshNormal = texture(inTexture3, fragCoord.xy).xyz;
			float mixFactor = result.type == sphereType ? 1.0 : 1.0;
			color = mix(color, vec3(1.0), meshNormal.x);
		}
	}
	
	fragColor = vec4(color, 1.0);
}

)""