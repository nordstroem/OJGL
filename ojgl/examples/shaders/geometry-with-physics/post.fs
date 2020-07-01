R""(
#version 430

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
	vec2 uv = fragCoord.xy;
    
    fragColor.r = texture(inTexture0, uv).r;
    float a = 0.5;
    vec2 dir = normalize(uv - vec2(0.5));
    float l = length(vec2(0.5) - uv);
    fragColor.g = texture(inTexture0, uv + dir * a * 0.01*l).g;
    fragColor.b = texture(inTexture0,  uv + dir * a * 0.02*l).b;
}

)""
