R""(
#version 430
#include "shaders/common/noise.fs"

in vec2 fragCoord;
in vec3 fragNormal;
in vec2 fragTexCoord;

out vec4 fragColor;

uniform float iTime;
uniform vec2 iResolution;
uniform mat4 iCameraMatrix;

uniform sampler2D inTexture0;


void main()
{
    float u = (fragCoord.x - 0.5) * iResolution.x / iResolution.y;
    float v = (fragCoord.y - 0.5);
	vec2 uv = vec2(u, v);
	vec3 uv3 = vec3(uv, 0.);
	float d = length(uv);
	float alpha = fbm3_high(uv3 * 2.5 + vec3(iTime), 0.16, 3.7);
	
	alpha *= 1.f - smoothstep(0.0, 0.5 + 0.1*noise_2(9*uv + vec2(cos(sin(iTime)) + iTime, sin(sin(iTime)))), d);
	fragColor = vec4(vec3(alpha), 1.0);
}

)""
