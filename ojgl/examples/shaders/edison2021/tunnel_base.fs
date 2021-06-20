R""(
#version 430
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

uniform sampler2D textTexture;
uniform sampler2D textTexture2;

const int sphereType = 1;
const int wallType = 2;
const int textType = 3;
const int floorType = 4;

// Pipes scene
const int pipesAType = 5;
const int pipesBType = 6;
const int starType = 7;

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

float sdBox2(vec3 p, vec3 b, inout vec2 uv)
{
    vec3 d = abs(p) - b;
    float dis = length(max(d, 0.0)) + min(max(d.x, max(d.y, d.z)), 0.0);

    uv = p.xy / (b.xy * 2) - 0.5;
    uv.x*=-1;
    return dis;
}

DistanceInfo edisonText(in vec3 p)
{
        vec2 uv;
        DistanceInfo box = {sdBox2(p, vec3(0.9, 0.25, 0.0), uv), textType };
        uv.x *=-1;

        if (true && box.distance < S_distanceEpsilon) {
            float s = texture(textTexture, uv).x;
            if (s > 0.2) { // If not on text
                box.type = textType;
                box.distance = 5000000;
            }
		}
        
        return box;
}

DistanceInfo pirateyText(in vec3 p)
{
        vec2 uv;
        DistanceInfo box = {sdBox2(p, vec3(0.6, 0.2, 0.0), uv), textType };
        uv.x *=-1;
        if (true && box.distance < S_distanceEpsilon) {
            float s = texture(textTexture2, uv).x;
            if (s > 0.2) { // If not on text
                box.type = textType;
                box.distance = 5000000;
            }
		}
        
        return box;
}


float cylinder(vec3 p, vec2 h)
{
	p.yz=p.zy;
  	vec2 d = abs(vec2(length(p.xz),p.y)) - h;
  	return min(max(d.x,d.y),0.0) + length(max(d,0.0));
}

vec3 tunnelDelta(float z)
{
    return vec3(3 * sin(0.03*z)*cos(0.1*z), 0.0, 0.0);
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

float getReflectiveIndex(int type)
{
    if (type == textType)
        return 0.1;
    if (type == sphereType)
        return 0.0;
    if (type == wallType)
        return 0.05;
    if (type == floorType)
        return 0.02;
    if (type == pipesAType || type == pipesBType)
        return 0.8;
    if (type == starType)
        return 0.8;
    return 0.0;
}

vec3 getAmbientColor(int type, vec3 pos) 
{
    vec3 wall = 3.0*vec3(0.2, 0.2, 0.2); 
    if (type == sphereType)
        return vec3(1.0);
    if (type == textType)
       return 145*palette(sin(pos.x*0.2) + 0.2*iTime, vec3(0.5), vec3(0.5), vec3(1.0, 1.0, 0.5), vec3(0.8, 0.9, 0.3));
       // return 145*vec3(0.0, 0.5, 1.0);
    if (type == wallType){
        //vec3 p = palette(0.05*pos.z, vec3(0.5), vec3(0.5), vec3(1.0, 1.0, 0.5), vec3(0.8, 0.9, 0.3));
        return wall;
    }
    if (type == floorType) {
        vec3 p = 0.1*palette(0.05*pos.z, vec3(0.5), vec3(0.5), vec3(1.0, 1.0, 0.5), vec3(0.8, 0.9, 0.3));
        return wall;//mix(p, wall, 0.04);
        //return vec3(0.5, 0.4, 0.7);
    }
    if (type == pipesAType) {
        return vec3(1, 0, 1);
    }
    if (type == pipesBType) {
        return vec3(0, 1, 1);
    }
    if (type == starType) {
        return 30 * vec3(1, 1, 1);
    }
    return vec3(0.1);
}

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
        //if (result.position.z < lightPosition.z)
        //    shadow = 0.3 + 0.7*shadowFunction(result.position, lightPosition, 32);
      
        float k = max(0.0, dot(rayDirection, reflect(invLight, normal)));
        float spec = 1 * pow(k, 5000.0);

        float diffuse = max(0., dot(invLight, normal)) * (1);

        if (result.type != wallType && result.type != floorType){
            invLight = normalize(vec3(1, 1, 1));
        
            float frontDiffuse = max(0., dot(invLight, normal));
            diffuse = max(diffuse, frontDiffuse);
          //  if (result.position.z > path(-6).z)
          //      lightStrength = max(1.0, lightStrength);
        }

        //        return vec3(ambient * (0.04 + 0.96*diffuse)) * result.transmittance + result.scatteredLight;

        return vec3(lightStrength * (ambient * (0.04 + 0.96*diffuse))) * result.transmittance * shadow + result.scatteredLight;
    } else {
        return vec3(0.0);
    }
}



void main()
{
    float u = (fragCoord.x - 0.5);
    float v = (fragCoord.y - 0.5) * iResolution.y / iResolution.x;

    float velocity = 4.0;

    rayOrigin = path(0);
    lookAt = path(-10.5);

    vec3 forward = normalize(lookAt - rayOrigin);
 	vec3 right = normalize(cross(forward, vec3(0.0, 1.0, 0.0)));   
    vec3 up = cross(right, forward);
    
    float fov = PI / 4.;
    rayDirection = normalize(u * right + v * up + fov * forward);

    
    //rayOrigin = (iCameraMatrix * vec4(u, v, -1.5, 1.0)).xyz;
   // eye = (iCameraMatrix * vec4(0.0, 0.0, 0.0, 1.0)).xyz;
   // rayDirection = normalize(rayOrigin - eye);

   eye = rayOrigin;
    vec3 color = march(eye, rayDirection);

    // Tone mapping
    color /= (color + vec3(1.0));

    fragColor = vec4(pow(color, vec3(0.4545)), 1.0);


   // float alpha = min(0.2, 1-texture(textTexture, fragCoord).x);
   // vec3 newColor = mix(fragColor.xyz, vec3(1.0), alpha);
   // fragColor = vec4(newColor, 1.0);


    // Add noise to reduce aliasing
    float noiseStrength = 1.0 - smoothstep(0.0, 0.4, length(fragColor.xyz));
    fragColor *= (1 + 0.2*noiseStrength*noise_2(vec2(u*1000,v*1000)));


}

)""
