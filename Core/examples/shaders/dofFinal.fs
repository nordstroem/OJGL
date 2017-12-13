R"(
#version 430

in vec2 fragCoord;
out vec4 fragColor;

uniform sampler2D inTexture0;
uniform sampler2D inTexture1;
uniform sampler2D inTexture2;

void main()
{
	vec2 uv = fragCoord.xy;
    if (uv.x < 0.5) {
        fragColor.rgb = texture(inTexture1, uv).rgb;
        fragColor.a = 1.;
    } else {
    	fragColor = texture(inTexture0, uv);
    }
    
    //fragColor = texture(inTexture0, uv);
	
}
)"