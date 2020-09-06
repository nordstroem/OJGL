R""(
#version 430

in vec2 fragCoord;
out vec4 fragColor;

uniform sampler2D inTexture0;

void main()
{
    fragColor = texture(inTexture0, fragCoord.xy);
}

)""
