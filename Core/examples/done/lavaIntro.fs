R""(
#version 430

in vec2 fragCoord;
out vec4 fragColor;

uniform float iTime;
uniform vec2 iResolution;

// Globals
vec3 lightPosition = vec3(4.0, 0, 4);

/// start of boiler-plate

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

float sdTorus(vec3 p, vec2 t)
{
	p.y *= 0.7;
  vec2 q = vec2(length(p.xy)-t.x,p.z);
  return length(q)-t.y;
}

float sdTorusJ(vec3 p, vec2 t)
{
	
  vec2 q = vec2(length(p.xy)-t.x,p.z);
  float d = length(q)-t.y;

	if (p.y > 0) {
		d = max(d, p.y);
	}
	return d;
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

float sdCappedCylinder( vec3 p, vec2 h )
{
  vec2 d = abs(vec2(length(p.xz),p.y)) - h;
  return min(max(d.x,d.y),0.0) + length(max(d,0.0));
}

// Smooth min. k determines smoothness
float smink( float a, float b, float k )
{
    float h = clamp( 0.5+0.5*(b-a)/k, 0.0, 1.0 );
    return mix( b, a, h ) - k*h*(1.0-h);

}
/// end of boiler-plate

float river(vec3 p) 
{
	float nv = noise((p.xy + vec2(iTime, 0)) * 3);
	vec3 noiseVec = 1*vec3(nv, 0, 0);
    float dis1 = sdBox(p + vec3(0.3*sin(p.y * 2.3), 0, -0.1) +  0.8*noiseVec, vec3(0.1, 5.0, 0.48));
	
	float height = sin(iTime);
	
	float dis =  sdTorus(p.yxz + 0.5*noiseVec - vec3(-0, 2, 0.56 + height), vec2(1, 0.01));
	//return sdTorusJ(p.yxz + vec3(0, 1, 0), vec2(1, 0.6));

	 dis = min(dis, sdCappedCylinder(p + 0.5*noiseVec - vec3(-2,1, 0.56 + height), vec2(0, 1)));
	// dis = min(dis, sdCappedCylinder(p.yxz - vec3(0.3, -0.2, 0.0), vec2(0.0, 1)));
	 dis = min(dis, sdTorusJ(p + 0.5*noiseVec - vec3(-1, 0, 0.56 + height), vec2(1, 0.02)));
	 return dis;
}

float map(vec3 p) 
{
	float noiseValue = noise(p.xy * 2 + 1*(p.xy + vec2(1.3,1.3))); 
	vec3 n = vec3(0, 0, noiseValue);
    float d1 = udRoundBox(p + 0.02 * n, vec3(100.0, 100.0, 0.5), 0.1);
	float d2 = river(p + vec3(10, 0, 0));
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
	u *= 16.0 / 9.0;

    vec3 ro = vec3(0, -3, 6.0);
    vec3 tar = vec3(0, 0, 0);
    vec3 dir = normalize(tar - ro);
	vec3 right = normalize(cross(vec3(0.0, 1.0, 0.0), dir));
	vec3 up = cross(dir, right);
	vec3 rd = normalize(dir + right*u + up*v);
                    
                    
    float t = 0.0;
    vec3 color = vec3(0.5);
	float transmittance = 1.0;
	vec3 scatteredLight = vec3(0.0);

    
    for (int i = 0; i < 100; i++) {
    	vec3 p = ro + rd * t;
        
        float d = map(p);
        float fogAmount = 0.005;
		float lol = river(p);
		vec3 light = 50.0 * vec3(1, 0.1, 0) / (lol*lol);
		//vec4 lightColDis = vec4(1, 0, 0, );
		//vec3 light = lightColDis.rgb;

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
            float str = 15.0/(0.1 + 0.2*dis + 0.01*dis*dis);
            color *= (0.75 + 0.25*diffuse);

			float lavaDistance = length(river(p));

			float lavaLight = 3 / (1 + 1 * lavaDistance + 500*lavaDistance * lavaDistance * lavaDistance);
			//color += vec3(min(0.1, lavaLight));

			color = transmittance * color + scatteredLight;
			//color *= str;
            break;
        }
        t += d;
    }    
	fragColor = vec4(color, 1.0);
    fragColor.rgb = fragColor.rgb / (fragColor.rgb + vec3(1.0));
}

)""  