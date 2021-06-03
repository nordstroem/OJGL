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

DistanceInfo sunk(DistanceInfo a, DistanceInfo b, float k) {
    DistanceInfo res = a.distance < b.distance ? a : b;
    res.distance = smink(a.distance, b.distance, k);
    return res;
}

vec3 rayOrigin;
vec3 rayDirection;

DistanceInfo map(in vec3 p)
{
    DistanceInfo walls = { -sdRoundBox(p, vec3(3.0, 5.0, 3.0), 0.2), wallType };
    DistanceInfo box = {sdBox(p - vec3(0, -1.5, 0), vec3(0.02, 5, 0.02)), wallType };

   // walls = sunk(walls, box, 0.2);
    p.xz *= rot(0.3*iTime);
    mo(p.xz, vec2(0.3, 0.5));
    p.x-=0.5;
    mo(p.xy, vec2(0.3, 0.5));
    p.yz *= rot(iTime);
    DistanceInfo sphere = {sdSphere(p - vec3(0.3, 0, 0), 0.1), sphereType};
    DistanceInfo frame = {sdBoxFrame(p - vec3(0.3, 0, 0), vec3(0.2), 0.004), sphereType};
    return un(sunk(sphere, frame, 0.3), walls);

}

float getReflectiveIndex(int type)
{
    return type == wallType ? 0.04 : 1-0 ;
}

vec3 getColor(in MarchResult result)
{
    vec3 lightPosition = vec3(0, 0, 0);
    if (result.type != invalidType) {
        float d = length(lightPosition - result.position);
        float lightStrength = 0.0005 / (0.000001 + 0.001*d*d*d);
        vec3 ambient = vec3(1.0, 1.0, 1.0);
        vec3 invLight = normalize(lightPosition - result.position);
        vec3 normal = normal(result.position);
        float shadow = shadowFunction(result.position, lightPosition, 32);

        float k = max(0.0, dot(rayDirection, reflect(invLight, normal)));
        float spec = 1 * pow(k, 20.0);

        float diffuse = max(0., dot(invLight, normal)) * (1);
        return vec3(lightStrength * (ambient * (0.04 + 0.96*diffuse) + spec)) * result.transmittance * shadow + result.scatteredLight;
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
    float r = 0.00001;
    mo(p.xz, vec2(0.3, 0.5));
    p.yz *= rot(-iTime);
    float d = sdBoxFrame(p- vec3(0.3, 0, 0), vec3(0.15, 0.15, 0.15 * psin(iTime) ), 0.01);

	d = max(0.001, d);

	float strength = 2;
	vec3 col = 0.2*palette(psin(iTime), vec3(0.5), vec3(0.5), vec3(1.0), vec3(0.0, 0.33, 0.67));//vec3(0.02, 0.08, 0.01);
	vec3 res = col * strength / (d * d);
	return VolumetricResult(d, res);
}

float getFogAmount(in vec3 p) 
{
    return 0.001;
}

void main()
{
    float u = (fragCoord.x - 0.5);
    float v = (fragCoord.y - 0.5) * iResolution.y / iResolution.x;
    rayOrigin = (iCameraMatrix * vec4(u, v, -1.0, 1.0)).xyz;
    vec3 eye = (iCameraMatrix * vec4(0.0, 0.0, 0.0, 1.0)).xyz;
    rayDirection = normalize(rayOrigin - eye);

    vec3 color = march(rayOrigin, rayDirection);

    // Tone mapping
    color /= (color + vec3(1.0));

    fragColor = vec4(pow(color, vec3(0.4545)), 1.0);

    // Add noise to reduce aliasing
    float noiseStrength = 1.0 - smoothstep(0.0, 0.4, length(fragColor.xyz));
    fragColor *= (1 + 0.2*noiseStrength*noise_2(vec2(u*1000,v*1000)));

    //fragColor = texture(textTexture, fragCoord.xy);

}

)""
