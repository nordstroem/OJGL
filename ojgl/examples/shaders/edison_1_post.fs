R""(
#version 430

in vec2 fragCoord;
out vec4 fragColor;

uniform sampler2D inTexture0;
uniform float iTime;

uniform float CHANNEL_11_SINCE;
uniform float CHANNEL_4_TO;
uniform float CHANNEL_4_TOTAL;

float smoothspike(float left, float right, float value) 
{
	float mid = (left + right) / 2.;
    return value < mid ? smoothstep(left, mid, value) : 1. - smoothstep(mid, right, value);
}


void main()
{
	vec2 uv = fragCoord.xy;
	    
    float beat = mod(iTime, 2.);
    float f = 1.*smoothspike(0.0, 0.45, beat);
    float r = texture(inTexture0, uv + f*vec2(0.02, 0.01)).r;
    float g = texture(inTexture0, uv + f*vec2(0.01, 0.01)).g;
    float b = texture(inTexture0, uv - f*vec2(0.01, 0.01)).b;
	vec3 org = texture(inTexture0, uv).rgb;
	vec3 per = vec3(r, g, b);
    //fragColor.rgb  = mix(org, per, 0.6); 
    //fragColor.rgb *=  0.9 + 0.1*clamp(sin(uv.y*1000.0) + 0.8, 0.0, 1.0);
	//fragColor.rgb *=  0.9 + 0.1*clamp(sin(uv.y*1000) + 0.8, 0.0, 1.0);


	fragColor.rgb = org;
}
)""