R""(
layout(location = 0) in vec3 position;
out vec2 fragCoord;

void main()
{
    fragCoord.x = (position.x + 1.0) * 0.5;
    fragCoord.y = (position.y + 1.0) * 0.5;
    gl_Position = vec4(position, 1.0);
}
)""
