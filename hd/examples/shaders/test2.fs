R""(
#version 430

in vec2 fragCoord;
out vec4 fragColor;

uniform float iTime;
uniform vec2 iResolution;

void main() {
    
    fragColor.rgb = vec3(fragCoord.x, 1.0, 0.0);
}

)""  