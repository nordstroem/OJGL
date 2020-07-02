R""(
#version 430
#include "common/noise.fs"
#include "common/primitives.fs"
#include "common/raymarch_utils.fs"
#include "common/utils.fs"

in vec2 fragCoord;
out vec4 fragColor;

uniform float iTime;
uniform vec2 iResolution;
uniform mat4 iCameraMatrix;

uniform sampler2D inTexture0;
uniform sampler2D inTexture1;

#define NUM_SCENES 6
float[] sceneLengths = float[NUM_SCENES](20., 10., 5., 5., 9, 14.);

int currentScene() 
{
    float s = iTime;
	for(int i = 0; i < NUM_SCENES; i++) {
		s-= sceneLengths[i];
		if (s < 0) return i;
	}
	return NUM_SCENES;
}

float localTime() {
	float s = iTime;
	for(int i = 0; i < NUM_SCENES; i++) {
		if (s - sceneLengths[i] < 0) return s;
		s-= sceneLengths[i];
	}
	return s;
}

float localTimeLeft() {
	float s = iTime;
	for(int i = 0; i < NUM_SCENES; i++) {
		if (s - sceneLengths[i] < 0) return sceneLengths[i] - s;
		s-= sceneLengths[i];
	}
	return 99999999999.;
}

float lTime = 0.;
float lTimeLeft = 0.;
int cScene = 0;

const int sphereType = 1;
const int wallType = 2;
const int groundType = 3;
const int ojType = 4;

DistanceInfo cannon(in vec3 p)
{
    float bt;
	float to = cScene == 5 ? 0. : 3.;
	float ft = mod(lTime - to, 2.0);

	float recoilLength = 0.2; 
	float recoil = 0.f;
	if ((cScene == 4 && lTime >= 3.) || (cScene == 5 && lTimeLeft > 11))
		recoil = recoilLength * smoothstep(0.0, 0.1, ft) * (1. - smoothstep(0.1, 2.0, ft));

	//float recoil = recoilLength * smoothstep(0.0, 0.1, ft) * (1. - smoothstep(0.1, 2.0, ft));
	//recoil = lTime >= to || cScene == 5 ? recoil : 0;  
	p.x += recoil;

	vec3 mp = p;
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


	float mTime = cScene == 4 ? lTime : 10;
	float d2 = sdRoundBox(mp, vec3(0.5 * smoothstep(0, 1, mTime)), 0.02);
	d = mix(d2, d, smoothstep(0., 3., mTime));

	DistanceInfo sphere1 = {d, sphereType };
    return sphere1;
}

DistanceInfo ground(in vec3 p)
{
	float d = 0.;
	if (cScene >= 4) {
		p.y += 0.92;
		d = sdRoundBox(p, vec3(3.0, 0.2, 3.0), 0.02);	
	} else {
		vec3 op = p;
		vec3 op2 = p;
		p.xz *= rot(-0.15*iTime + 5.2);
		
		d = sdSphere(p, 2.) + 0.005*fbm3_high(10.*p, 0.85, 2.2) + 0.08*fbm3_high(0.1*p, 1.9, 2.9);

		if (cScene == 0 && lTimeLeft > 3.0) {
			op.xz *= rot(-0.15 * iTime + 4.9);
			float indent = sdTorus(p.xzy - vec3(0, -1.95, 0.), vec2(0.3, 0.015));
			op.z += 1.96;
			float indent2 = sdCappedCylinder(op - vec3(-0.2, 0.1, -0.01), vec2(0.018, 0.23));
			indent2 = min(indent2, sdHalfTorus(op - vec3(0, -0.1, 0.), vec2(0.2, 0.018)));
			indent2 = min(indent2, sdCappedCylinder(op.yxz - vec3(0.235, -0.055, 0.005), vec2(0.015, 0.16)));

			indent = min(indent, indent2);
			d = max(d, -indent);
		}

		if (cScene == 3) {
			float d3 = sdRoundBox(op2 + vec3(0, 0.92, 0), vec3(3.0, 0.2, 3.0), 0.02);	
			d = mix(d, d3, smoothstep(2, 4, lTime));
		}
		

	}
	
	DistanceInfo plane = {d, groundType};
	return plane;
}

DistanceInfo map(in vec3 p)
{
	if (cScene >= 4) {
		return un(ground(p), cannon(p));
	}
	return ground(p);
}

float getReflectiveIndex(int type)
{
	if (type == groundType) return 0.2;
	if (type == sphereType) return 0.5;
	return 0.0;
}

float getSpecularFactor(int type)
{
    return type == groundType ? 1.0: 0.3;
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
	float k = max(0.0, dot(rayDirection, reflect(invLight, normal)));
    float spec = getSpecularFactor(result.type) * pow(k, 20.0);
	float ao = genAmbientOcclusion(result.position, rayDirection).x;
	
	float l = length(result.position - vec3(0.6,0,0));
	float darkness = 1. / (0.1 + 0.2*pow(l, 2.));
    if (result.type != invalidType) {
		if (result.type == wallType)
			return 0.3*vec3(1.0);
		else if (result.type == groundType) {
			return diffuse*ao*vec3(1.0, 0.2, 0.2) + spec;
		} else {
		    vec3 ambient = 1.3*vec3(0.1, 0.1, 0.1);
			return vec3(ambient + 0.6*diffuse - result.steps / 100.f + 0.7*spec);
		}
    } else {
		float tau = 2. * PI;
		float phi = 0.5 + atan(rayDirection.x, rayDirection.z) / tau - 0;
		float theta = 0.5 - asin(rayDirection.y) / PI;
	    float u2 = cos(2*tau*sin(3*tau*phi)) + cos(0.3 + 2.*tau*phi) + sin(5.*theta);
		u2 = pow(0.9*fbm3_high(10.*vec3(psin(tau * phi), psin(tau * theta), 0.02*iTime), 0.85, 2.2), 3.0);
        return 0.015*vec3(u2, u2, u2);
    }
}

vec3 getMeshColor(in vec3 pos, in vec3 normal, in vec3 rayDirection)
{
	vec3 invLight = -normalize(vec3(0.5, -0.7, -0.3));
	float diffuse = max(0., dot(invLight, normal));
    vec3 ambient = vec3(0.9, 0.9, 0.9);
    return 3.*ambient * (0.5 + 0.5*diffuse);
}

void main()
{
	lTime  = localTime();
	cScene = currentScene();
	lTimeLeft = localTimeLeft();

    float u = (fragCoord.x - 0.5) * iResolution.x / iResolution.y;
    float v = (fragCoord.y - 0.5);
    vec3 rayOrigin = (iCameraMatrix * vec4(u, v, -1.0, 1.0)).xyz;
    vec3 eye = (iCameraMatrix * vec4(0.0, 0.0, 0.0, 1.0)).xyz;
    vec3 rayDirection = normalize(rayOrigin - eye);

    MarchResult result = march(eye, rayDirection);
    vec3 color = getMarchColor(result, rayDirection);
	
    float reflectiveIndex = getReflectiveIndex(result.type);
    if (reflectiveIndex > 0.) {
        rayDirection = reflect(rayDirection, normal(result.position));
        MarchResult reflectResult = march(result.position + 0.1 * rayDirection, rayDirection);
        vec3 newColor = getMarchColor(reflectResult, rayDirection);
        color = mix(color, newColor, reflectiveIndex);
    }


	if (abs(texture(inTexture0, fragCoord.xy).w - 1.0) < 0.01) {
		vec3 meshPos = texture(inTexture0, fragCoord.xy).xyz;
		
		if (result.type == invalidType || length(eye - meshPos) < length(eye - result.position)) {
			vec3 meshNormal = texture(inTexture1, fragCoord.xy).xyz;
			color = mix(color, getMeshColor(meshPos, meshNormal, rayDirection), 1.);
		}
	}

	float f = 1.;
	float focus = 0.f;

	if (cScene == 0) {
		color = mix(vec3(1.), color, smoothstep(0., 3., lTime));
		f *= 1. - smoothstep(3.5f, 3.f, lTimeLeft);
		f += smoothstep(3.f, 2.5, lTimeLeft);
		focus = 1.f - smoothstep(2, 4, lTime) + abs(length(result.position - eye) - 6.) * 0.1;
	} 
	if (cScene == 1 || cScene == 2 || (cScene == 0 && lTimeLeft < 3)) {
		focus = abs(length(result.position - eye) - 9.) * 0.13;
	}else if (cScene == 3 || cScene == 4) {
		focus = 0.1;
	}
	if ((cScene == 4 && lTimeLeft < 3) ||cScene == 5) {
		focus =  0.1;
	}

	if (cScene == 5) {
		f = 1. - smoothstep(5.5f, 0.5, lTimeLeft);
	}


	fragColor = vec4(f * color, focus);
}

)""
