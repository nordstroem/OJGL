R""(
#version 430
#include "common/noise.fs"
#include "common/primitives.fs"
#include "edison2021/raymarch_settings.fs"
#include "common/raymarch_utils.fs"
#include "common/utils.fs"

in vec2 fragCoord;
out vec4 fragColor;

uniform float iTime;
uniform vec2 iResolution;
uniform mat4 iCameraMatrix;

uniform sampler2D textTexture;

const int sphereType = 1;
const int wallType = 2;
const int textType = 3;

DistanceInfo sunk(DistanceInfo a, DistanceInfo b, float k) {
    DistanceInfo res = a.distance < b.distance ? a : b;
    res.distance = smink(a.distance, b.distance, k);
    return res;
}

vec3 rayOrigin;
vec3 rayDirection;

float sdBox2(vec3 p, vec3 b, inout vec2 uv)
{
    vec3 d = abs(p) - b;
    float dis = length(max(d, 0.0)) + min(max(d.x, max(d.y, d.z)), 0.0);

    uv = p.xy / (b.xy * 2) - 0.5;

    return dis;
}

DistanceInfo edisonText(in vec3 p)
{
        vec2 uv;
        DistanceInfo box = {sdBox2(p, vec3(1, 1, 0.0), uv), textType };

        if (box.distance < S_distanceEpsilon) {
            float s = texture(textTexture, uv + vec2(0., 0.12)).x;
            if (s > 0.2) { // If not on text
                box.type = textType;
                box.distance = 5000000;
            }
		}

        return box;
}


float cylinder(vec3 p, vec2 h)
{
	p.yz=p.zy;
  	vec2 d = abs(vec2(length(p.xz),p.y)) - h;
  	return min(max(d.x,d.y),0.0) + length(max(d,0.0));
}
DistanceInfo map(in vec3 p)
{
    vec3 path = vec3(10 * sin(0.02*p.z)*cos(0.2*p.z), 0.0, 0.0);
    p = p - path;
    DistanceInfo box = {-sdBox(p, vec3(0.7, 0.7, 1000)), sphereType };
    DistanceInfo box2 = {sdBox(p, vec3(0.3)), wallType };
    
    

    return un(box, box2);
}


float getReflectiveIndex(int type)
{
    if (type == textType)
        return 0.0;
    if (type == sphereType)
        return 0.0;
    if (type == wallType)
        return 0.0;

    return 0.0;
}

vec3 getAmbientColor(int type) 
{
    if (type == sphereType)
        return vec3(0.5, 0.3, 0.1);
    if (type == textType)
        return vec3(0.0);
    return vec3(0.1);
}

vec3 getColor(in MarchResult result)
{
    vec3 lightPosition = vec3(0, 0, 0.0);
    if (result.type != invalidType) {
        float d = length(lightPosition - result.position);
        float lightStrength = 1.0;//0.0005 / (0.000001 + 0.001*d*d);
        vec3 ambient = getAmbientColor(result.type);
        vec3 invLight = normalize(lightPosition - result.position);
        vec3 normal = normal(result.position);
        float shadow = 1.0;//shadowFunction(result.position, lightPosition, 32);

        float k = max(0.0, dot(rayDirection, reflect(invLight, normal)));
        float spec = 1 * pow(k, 20.0);

        float diffuse = max(0., dot(invLight, normal)) * (1);
        return vec3(lightStrength * (ambient * (0.04 + 0.96*diffuse))) * result.transmittance * shadow + result.scatteredLight;
    } else {
        return vec3(0.0);
    }
}

vec3 palette( in float t, in vec3 a, in vec3 b, in vec3 c, in vec3 d )
{
    return a + b*cos( 6.28318*(c*t+d) );
}

VolumetricResult evaluateLight(in vec3 p)
{
    float d = sdBox(p, vec3(0.01));

	d = max(0.001, d);

	float strength = 2;
	vec3 col = vec3(0.02, 0.08, 0.01);
	vec3 res = col * strength / (d * d);
    
	return VolumetricResult(d, res);
}

float getFogAmount(in vec3 p) 
{
    return 0.0;
}

void main()
{
    float u = (fragCoord.x - 0.5);
    float v = (fragCoord.y - 0.5) * iResolution.y / iResolution.x;
    rayOrigin = (iCameraMatrix * vec4(u, v, -1.0, 1.0)).xyz;
    vec3 eye = (iCameraMatrix * vec4(0.0, 0.0, 0.0, 1.0)).xyz;
    rayDirection = normalize(rayOrigin - eye);

    vec3 color = march(eye, rayDirection);

    // Tone mapping
    color /= (color + vec3(1.0));

    fragColor = vec4(pow(color, vec3(0.4545)), 1.0);


   // float alpha = min(0.2, 1-texture(textTexture, fragCoord).x);
   // vec3 newColor = mix(fragColor.xyz, vec3(1.0), alpha);
   // fragColor = vec4(newColor, 1.0);


    // Add noise to reduce aliasing
    float noiseStrength = 1.0 - smoothstep(0.0, 0.4, length(fragColor.xyz));
    fragColor *= (1 + 0.2*noiseStrength*noise_2(vec2(u*1000,v*1000)));


}

)""
