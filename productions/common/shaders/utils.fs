R""(
#define PI 3.14159265
float pMod1(inout float p, float size)
{
    float halfsize = size * 0.5;
    float c = floor((p + halfsize) / size);
    p = mod(p + halfsize, size) - halfsize;
    return c;
}
vec2 pMod2(inout vec2 p, vec2 size)
{
    vec2 c = floor((p + size * 0.5) / size);
    p = mod(p + size * 0.5, size) - size * 0.5;
    return c;
}

float pModPolar(inout vec2 p, float repetitions)
{
    float angle = 2 * PI / repetitions;
    float a = atan(p.y, p.x) + angle / 2.;
    float r = length(p);
    float c = floor(a / angle);
    a = mod(a, angle) - angle / 2.;
    p = vec2(cos(a), sin(a)) * r;
    if (abs(c) >= (repetitions / 2))
        c = abs(c);
    return c;
}
mat2 rot(float a)
{
    return mat2(cos(a), sin(a), -sin(a), cos(a));
}




float psin(float v){
	return 0.5 * (1.0 + sin(v));
}
float smink( float a, float b, float k )
{
    float h = clamp( 0.5+0.5*(b-a)/k, 0.0, 1.0);
    return mix( b, a, h ) - k*h*(1.0-h);
}




)""
