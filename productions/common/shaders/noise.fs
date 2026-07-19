R""(
float hash11(float p)
{
    return fract(sin(mod(p * 727.1, 3.14159265)) * 435.545);
}
float hash12(vec2 p)
{
    float h = dot(p, vec2(127.1, 311.7));
    return fract(sin(mod(h, 2 * 3.14159265)) * 437.545);
}
vec3 hash31(float p)
{
    vec3 h = vec3(127.231, 491.7, 718.423) * p;
    return fract(sin(mod(h, 2 * 3.14159265)) * 435.543);
}
float noise_2(in vec2 p)
{
    vec2 i = floor(p);
    vec2 f = fract(p);
    vec2 u = f * f * (3.0 - 2.0 * f);
    return mix(mix(hash12(i + vec2(0.0, 0.0)), hash12(i + vec2(1.0, 0.0)), u.x), mix(hash12(i + vec2(0.0, 1.0)), hash12(i + vec2(1.0, 1.0)), u.x), u.y);
}
float noise_3(in vec3 p)
{
    vec3 i = floor(p);
    vec3 f = fract(p);
    vec3 u = f * f * (3.0 - 2.0 * f);

    vec2 ii = i.xy + i.z * vec2(5.0);
    float a = hash12(ii + vec2(0.0, 0.0));
    float b = hash12(ii + vec2(1.0, 0.0));
    float c = hash12(ii + vec2(0.0, 1.0));
    float d = hash12(ii + vec2(1.0, 1.0));
    float v1 = mix(mix(a, b, u.x), mix(c, d, u.x), u.y);

    ii += vec2(5.0);
    a = hash12(ii + vec2(0.0, 0.0));
    b = hash12(ii + vec2(1.0, 0.0));
    c = hash12(ii + vec2(0.0, 1.0));
    d = hash12(ii + vec2(1.0, 1.0));
    float v2 = mix(mix(a, b, u.x), mix(c, d, u.x), u.y);

    return max(mix(v1, v2, u.z), 0.0);
}
float fbm3(vec3 p, float a, float f)
{
    return noise_3(p);
}
float fbm3_high(vec3 p, float a, float f)
{
    float ret = 0.0;
    float amp = 1.0;
    float frq = 1.0;
    for (int i = 0; i < 4; i++) {
        float n = pow(noise_3(p * frq), 2.0);
        ret += n * amp;
        frq *= f;
        amp *= a * (pow(n, 0.2));
    }
    return ret;
}
)""
