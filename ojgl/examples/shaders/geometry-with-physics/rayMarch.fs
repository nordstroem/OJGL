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
    DistanceInfo walls = { -sdBox(p, vec3(2.0, 2.0, 4.0)), wallType };
    DistanceInfo sphere2 = { sdSphere(p - vec3(1.0, 1.0, -1.0), 0.1), sphereType };
    pModPolar(p.xy, 5);
    p.x -= 0.6;
    DistanceInfo sphere1 = { sdSphere(p, 0.25) + 0.05 * noise_3(p.xyz * 10.0 + iTime), sphereType };
    return un(un(sphere1, sphere2), walls);
}

float getReflectiveIndex(int type)
{
    return type == wallType ? 0.6 : 0.0;
}

vec3 getMarchColor(in MarchResult result)
{
    if (result.type != invalidType) {
        vec3 ambient = vec3(0.1, 0.1, 0.1);
        //vec3 invLight = -normalize(vec3(-0.7, -0.2, -0.5));
		vec3 invLight = -normalize(vec3(0,-3,0) - result.position);
        vec3 normal = normal(result.position);
        float diffuse = max(0., dot(invLight, normal));
        return vec3(ambient * (diffuse));
    } else {
        return vec3(0.0);
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
