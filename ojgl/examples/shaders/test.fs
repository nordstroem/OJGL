R""(
#version 430

in vec2 fragCoord;
out vec4 fragColor;

uniform float iTime;
uniform vec2 iResolution;

void main() {
    
    fragColor.rgb = vec3(0.5 + 0.5 * sin(iTime), fragCoord.x, 0.0);
}

)""  