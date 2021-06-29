R""(
#include "common/noise.fs"
#include "common/primitives.fs"
#include "edison2021/raymarch_settings.fs"
#include "common/raymarch_utils.fs"
#include "common/utils.fs"

in vec2 fragCoord;
out vec4 fragColor;

uniform float iTime;
uniform float iAbsoluteTime;
uniform vec2 iResolution;
uniform mat4 iCameraMatrix;

const int sphereType = 1;
const int wallType = 2;
const int floorType = 3;
const int lastBaseType = 4;

DistanceInfo sunk(DistanceInfo a, DistanceInfo b, float k) {
    DistanceInfo res = a.distance < b.distance ? a : b;
    res.distance = smink(a.distance, b.distance, k);
    return res;
}

vec3 palette( in float t, in vec3 a, in vec3 b, in vec3 c, in vec3 d )
{
    return a + b*cos( 6.28318*(c*t+d) );
}

vec3 eye;
vec3 rayOrigin;
vec3 rayDirection;
vec3 lookAt;

vec3 tunnelDelta(float z)
{
#ifdef CUSTOM_DELTA_MODIFIER
    const float deltaModifier = tunnelDeltaModifier();
#else
    const float deltaModifier = 3;
#endif
    return vec3(deltaModifier * sin(0.03*z)*cos(0.1*z), 0.0, 0.0);
}

vec3 path(float zDelta) {
    float period = 200 * PI; // (depends on tunnelDelta)
    float velocity = 10.0;
    vec3 pos = vec3(0.0, 0.0, mod(-iAbsoluteTime * velocity, period) + zDelta);
    pos += tunnelDelta(pos.z);
    return pos;
}

float filteredLines(float y, float dp)
{
    float i = 2.0*(abs(fract((y-0.5*dp)*0.5)-0.5)-
                  abs(fract((y+0.5*dp)*0.5)-0.5))/dp;
    return 0.5 - 0.5*i*i;                  
}

float getReflectiveIndex(int type);
vec3 getAmbientColor(int type, vec3 pos);


vec3 getColor(in MarchResult result)
{
    if (result.type != invalidType) {
        vec3 lightPosition = path(-11.5);
        float d = length(lightPosition - result.position);
        
        float lightStrength =  0.0002 / (0.000001 + 0.0005*d*d );

        vec3 ambient = getAmbientColor(result.type, result.position);
        vec3 invLight = normalize(lightPosition - result.position);
        vec3 normal = normal(result.position);
        float shadow = 1.0;
      
        float k = max(0.0, dot(rayDirection, reflect(invLight, normal)));
        float spec = 1 * pow(k, 5000.0);

        float diffuse = max(0., dot(invLight, normal)) * (1);

        if (result.type != wallType && result.type != floorType){
            invLight = normalize(vec3(1, 1, 1));
        
            float frontDiffuse = max(0., dot(invLight, normal));
            diffuse = max(diffuse, frontDiffuse);
        }
        return vec3(lightStrength * (ambient * (0.04 + 0.96*diffuse))) * result.transmittance * shadow + result.scatteredLight;
    } else {
        return vec3(0.0);
    }
}


void main()
{
    float u = (fragCoord.x - 0.5);
    float v = (fragCoord.y - 0.5) * iResolution.y / iResolution.x;

    rayOrigin = path(0);
    lookAt = path(-10.5);

    vec3 forward = normalize(lookAt - rayOrigin);
 	vec3 right = normalize(cross(forward, vec3(0.0, 1.0, 0.0)));   
    vec3 up = cross(right, forward);
    
    float fov = PI / 4.;
    rayDirection = normalize(u * right + v * up + fov * forward);

    eye = rayOrigin;
    vec3 color = march(eye, rayDirection);

    // Tone mapping
    color /= (color + vec3(1.0));

    fragColor = vec4(pow(color, vec3(0.4545)), 1.0);

    // Add noise to reduce aliasing
    float noiseStrength = 1.0 - smoothstep(0.0, 0.4, length(fragColor.xyz));
    fragColor *= (1 + 0.2*noiseStrength*noise_2(vec2(u*1000,v*1000)));


}

)""
