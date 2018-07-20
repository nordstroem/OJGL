R""(
#version 430

in vec2 fragCoord;
out vec4 fragColor;

uniform float iTime;
uniform vec2 iResolution;

// Globals
vec3 lightPosition = vec3(4.0, 0, 4);

/// start of boiler-plate

float timeScene0 = 8-8;
float timeScene1 = 16-16;
float timeScene2 = 40-10;

mat3 rotateZ(float theta) {
    float c = cos(theta);
    float s = sin(theta);

    return mat3(
        vec3(c, -s, 0),
        vec3(s, c, 0),
        vec3(0, 0, 1));

}

float sdSphere(vec3 p, float s)
{
  return length(p) - s;
}

float udRoundBox(vec3 p, vec3 b, float r)
{
  return length(max(abs(p)-b,0.0)) - r;
}

float sdBox( vec3 p, vec3 b )
{
  vec3 d = abs(p) - b;
  return min(max(d.x,max(d.y,d.z)),0.0) + length(max(d,0.0));
}

float hash( in vec2 p ) {
    float h = dot(p,vec2(127.1,311.7));  
    return fract(sin(h)*43758.5453123);
}

float noise( in vec2 p ) {
    vec2 i = floor( p );
    vec2 f = fract( p );  
    vec2 u = f*f*(3.0-2.0*f);
    return mix( mix( hash( i + vec2(0.0,0.0) ),
                     hash( i + vec2(1.0,0.0) ), u.x),
                mix( hash( i + vec2(0.0,1.0) ),
                     hash( i + vec2(1.0,1.0) ), u.x), u.y);
}

float noiseOctave(in vec2 p, int octaves, float persistence)
{
    float n = 0.;
    float amplitude = 1.;
    float frequency = 1.;
    float maxValue = 0.;

    for(int i = 0; i < octaves; i++)
    {
        n += noise((p+float(i)) * frequency) * amplitude;
        maxValue += amplitude;
        amplitude *= persistence;
        frequency *= 2.0;
    }
    return n / maxValue;
}

/// end of boiler-plate

float lines(vec3 p, float time)
{
    float k1 = smoothstep(timeScene1 + 2, timeScene2, time);
    float k2 = smoothstep(timeScene1 + 8, timeScene2, time);
    float k3 = smoothstep(timeScene1 + 30, timeScene1+40, time);
    float nv = noise((p.xy + vec2(iTime, 0)) * 3);
    vec3 noiseVec = 1*vec3(nv, 0, 0);
    float d1 = sdBox(p + vec3(0.3*sin(p.y * 2.3), 0, -0.1) +  0.8*noiseVec, vec3(0.2, 25.0 * k1, 0.48));
    //time *= 1.5;s
    float d2 = sdBox(p.yxz + vec3(0.3*sin(p.y * 2.3), 0, -0.1) +  0.8*noiseVec, vec3(0.2, 25.0 * k2, 0.48));
    float d3 = sdBox(p.xzy + vec3(0.3*sin(p.y * 2.3), 0, -0.1) +  0.8*noiseVec, vec3(0.01, 25.0 * k3, 0.1));
    return min(min(d1, d2), 6000*d3);
}

float river(vec3 p)
{
    float time = mod(iTime, 65);
    float k = smoothstep(timeScene1+20, timeScene1+25, time);
    float th = 0.2*log(1 + pow(2.17, time - timeScene1-20));
    mat3 mf = rotateZ(th);
    mat3 m = rotateZ(3.14/4);
    mat3 m2 = rotateZ(3.14/2);

    float d1 = lines(p, time);
    float d2 = lines(inverse(m)*p, time - 15);
    float d3 = lines(inverse(m2)*p, time - 25);
    return min(d1, min(d2, 600*d3));
}

float map(vec3 p)
{
    float noiseValue = noise(p.xy * 2 + 1*(p.xy + vec2(1.3,1.3)));
    vec3 n = vec3(0, 0, noiseValue);
    float d1 = udRoundBox(p + 0.02 * n, vec3(100.0, 100.0, 0.5), 0.1);
    float d2 = river(p);
    return max(d1, -d2);
}

vec3 getNormal(vec3 p) {
    vec3 ep = vec3(0.01, 0, 0);
    vec3 normal;
    normal.x = map(p + ep.xyz) - map(p - ep.xyz);
    normal.y = map(p + ep.yxz) - map(p - ep.yxz);
    normal.z = map(p + ep.yzx) - map(p - ep.yzx);
    return normalize(normal);
}

void main() {
    float u = fragCoord.x * 2.0 - 1.0;
    float v = fragCoord.y * 2.0 - 1.0;

    float time = mod(iTime, 65);
    //time = 0;
    float k = smoothstep(timeScene0, timeScene0+0.05, time);
    float k2 = smoothstep(timeScene1+5, timeScene2+13, time);
    float k3 = smoothstep(timeScene1, timeScene1+1, time);
    float k4 = smoothstep(timeScene1, timeScene1 + 5, time);
    float k5 = smoothstep(timeScene1 + 32, timeScene1 + 50, time);

    u *= 16.0 / 9.0;
    u *= 1 + 0*(k - k3)*0.3*sin(cos(time));
    v *= 1 + 0*(k - k3)*0.5*cos(cos(time));

    vec3 ro = vec3(0.3 - cos(time) + (k - k3)*0*sin(time*40)*0.5*cos(70*sin(cos(time))), k5*10+(-8 - 5*k2) + (k - k3)*0*0.1*sin(30*time), (4.0 + k2 * 20));
    //vec3 ro = vec3(10*cos(time), 10*sin(time), 10);
    vec3 tar = vec3(0.1*k * sin(time- timeScene0), 3, 0.1*k*sin(time- timeScene0));
    vec3 dir = normalize(tar - ro);
    vec3 right = normalize(cross(vec3(0.0, 0.0, 1.0), dir));
    vec3 up = cross(dir, right);
    vec3 rd = normalize(dir + right*u + up*v);
                  
                  
    float t = 0.0;
    vec3 color = vec3(0.5);
    float transmittance = 1.0;
    vec3 scatteredLight = vec3(0.0);

    //time = timeScene1 + 3;
  
    for (int i = 0; i < 100; i++) {
        vec3 p = ro + rd * t;
      
        float d = map(p);
        float fogAmount = 0.005;
        float lol = river(p);
        float str = 50 - 48 * 1 + k5 * 5;
        vec3 light = str * vec3(1, 0.1 + (1-k4)*(1 + sin(time))*0.1, 0) / (lol*lol);
        //vec4 lightColDis = vec4(1, 0, 0, );
        //vec3 light = lightColDis.rgb;
        d = min(d, lol);
        vec3 lightIntegrated = light - light * exp(-fogAmount * d);
        scatteredLight += transmittance * lightIntegrated;  
        transmittance *= exp(-fogAmount * d);
      
        if (d < 0.01 || i == 99) {
            color = vec3(0.01);
       
            float dis = length(river(p) - p);
            vec3 invLight = normalize(lightPosition - p);
          
            vec3 normal = getNormal(p);
          
            float diffuse = max(0.0, dot(invLight, normal));
            float s = 10.0;
            float k = max(0.0, dot(rd, reflect(invLight, normal)));
            float spec =  pow(k, s);
            float str = 40.0/(0.1 + 0.2*dis + 0.04*dis*dis);
            color *= (0.75 + 0.25*diffuse);

            float lavaDistance = length(river(p));

            float lavaLight = 3 / (1 + 1 * lavaDistance + 500*lavaDistance * lavaDistance * lavaDistance);
            //color += vec3(min(0.1, lavaLight));

            color = transmittance * color + scatteredLight;
            color *= str;
			break;
        }
        t += d;
    }
    float fstr = smoothstep(46, 50, time);
    float fstr2 = smoothstep(0, 2, time);
    fragColor = vec4(color, 1.0);
    fragColor.rgb = fstr2 * (1 - fstr) * smoothstep(0, timeScene0, time) * fragColor.rgb / (fragColor.rgb + vec3(1.0));
}	

)""  