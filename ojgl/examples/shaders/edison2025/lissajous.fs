R""(


uniform sampler2D inTexture0;
uniform float iTime;
uniform vec2 iResolution;
in vec2 fragCoord;
out vec4 fragColor;

void main()
{
    float u = (fragCoord.x - 0.5);
    float v = (fragCoord.y - 0.5) * iResolution.y / iResolution.x;
    vec2 uv = vec2(u, v);
    
    float t = 0.3*iTime;
    float A = 0.5;
    float B = 0.5;
    float delta = 3.14159265 / 2;
    float a = 3;
    float b = 2;
    float x = 0.5 * (A * sin(a * t + delta));
    float y = 0.5 * (B * sin(b * t));
    vec2 luv = vec2(x, y);
    vec3 color;
    vec3 prevColor = texture(inTexture0, fragCoord).xyz;
    
    if (length(uv - luv) < 0.01) {
        color = vec3(1.0, 0.0, 0.0);
    } else {
        color = mix(prevColor, vec3(0.0, 0.0, 0.0), 0.01);
    }


    fragColor = vec4(color, 1);
}

)""
