R""(
#version 430

in vec2 fragCoord;
out vec4 fragColor;

uniform sampler2D inTexture0;

uniform vec2 iResolution;


const int nsamples = 10;

void main()
{
    vec2 center = vec2(0.5, 0.5);
	float blurStart = 1.0;
    float blurWidth = 0.03;

    
    float u = (fragCoord.x);
    float v = (fragCoord.y);

	vec2 uv = vec2(u, v);
    
    uv -= center;
    float precompute = blurWidth * (1.0 / float(nsamples - 1));
    
    vec4 color = vec4(0.0);
    for(int i = 0; i < nsamples; i++)
    {
        float scale = blurStart + (float(i)* precompute);
        color += texture(inTexture0, uv * scale + center);

    }
    
    
    color /= float(nsamples);
    
	fragColor = color;
}

)""
