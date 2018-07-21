R""(
#version 430

in vec2 fragCoord;
out vec4 fragColor;

uniform float iTime;
uniform vec2 iResolution;

// Globals
vec3 lightPosition = vec3(-4.0, 5, -4);

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

float smin( float a, float b)
{
	float k = 2.0;
    float h = clamp( 0.5+0.5*(b-a)/k, 0.0, 1.0 );
    return mix( b, a, h ) - k*h*(1.0-h);
}

vec4 sun(vec4 a, vec4 b)
{
	float sm = smin(a.x,b.x);
	float ca = abs(sm -a.x);
	float cb = abs(sm -b.x);

	return ca < cb ? vec4(sm, a.yzw) : vec4(sm, b.yzw);
}

mat3 rotateZ(float theta) {
    float c = cos(theta);
    float s = sin(theta);

    return mat3(
        vec3(c, -s, 0),
        vec3(s, c, 0),
        vec3(0, 0, 1));
}

/// end of boiler-plate

float river(vec3 p) 
{
	float nv = noise((p.xy + vec2(iTime, 0)) * 3);
	vec3 noiseVec = 1*vec3(nv, 0, 0);
	//float dis = sdCappedCylinder(p - vec3(sin(iTime*20), 0.6*cos(sin(iTime)*10), sin(iTime)), vec2(1 + 0.03*sin(20*p.y + 200*iTime) + 0.6 + 1*sin(0.3*p.y), 5.0));
	float dis = sdCappedCylinder(p - vec3(0, 5, 2), vec2(abs(0.5*p.y), 5.0));
	return dis;
}

// a < b
float inverseMix(float value, float a, float b)
{
	return clamp((value - a) / (b - a), 0, 1);
}

float posSin(float value) 
{
	return 0.5 * (1 + sin(value));
}

vec3 tornadoPos(float time)
{
	return vec3(0);//vec3(10*cos(2*sin(0.5*time) + 3*sin(time + noise(vec2(time)))), -5, 10*sin(0.5*time + 2*cos(0.5*time)));
}

float cros(vec3 p)
{
	float time = mod(iTime, 40);
	//time += 19;
	float k1 = smoothstep(6, 7, time);
	float k2 = smoothstep(17, 18, time);
	float k3 = smoothstep(20.4, 21.5, time);
	float k4 = smoothstep(23.0, 24.0, time);
	float k5 = smoothstep(30.0, 40.0, time);
	float k6 = smoothstep(20.0, 37.0, time);
	float nv = noise((p.xy + vec2(iTime, 0)) * 3);
	float d = sdCappedCylinder(p, vec2(0.05 -k6*2*nv+ 0.3*nv, 4.0+ (k2 - k3)*0)); 
	float d2 = sdCappedCylinder(p.zxy, vec2(0.05 -k6*2*nv+ 0.3*nv, 4.0+ (k2 - k3)*0)); 
	return min(d, d2);
}

vec4 disMin(vec4 a, vec4 b)
{
	return a.x < b.x ? a : b;

}
vec3 symbolPos(float time)
{
	return vec3(0, 3, 14);
}

vec4 symbol(vec3 p)
{
	float time = mod(iTime, 40);
	//time += 19;
	float k1 = smoothstep(6, 7, time);
	float k2 = smoothstep(20, 21, time);
	mat3 mm = rotateZ(0.3*log(1 + pow(2.17, -(time -15))));


	p -= symbolPos(iTime);
	float d = cros(inverse(mm)*p);
	mat3 m = rotateZ(3.14/4);
	float d2 = cros(inverse(mm)*inverse(m)*p);

	//float nv = noise((p.xy + p.xz+ vec2(0.06*sin(5*iTime), 0)) * 3);
	//float d3 = sdBox(p - vec3(0, -9.9 + nv, 10), vec3(20, 0.001, 0.5));
	//d3 = min(d3, sdBox(p - vec3(-20, -9.9 + nv, -10), vec3(0.5, 0.001, 20)));
	//d3 = min(d3, sdBox(p - vec3(20, -9.9 + nv, -10), vec3(0.5, 0.001, 20)));

	vec3 c1 = vec3(5,3,3);
	vec3 q1 = mod(p, c1)- 0.5*c1;
	return vec4(min(d,  d2), 1, 0.1, 0.002);
}




vec4 map(vec3 p, vec3 rd) 
{
	float nv = noise((p.xy + p.xz + p.yz) * 3);
	float dis = -udRoundBox(p - vec3(0, 1, 0), vec3(23, 5, 22), 5);
	vec4 box = vec4(dis, vec3(0.3, 0.3, 0.3));
	vec3 c1 = vec3(3,3,3);
	vec3 q1 = mod(p, c1)- 0.5*c1;
	vec4 floor = vec4(sdBox(vec3(q1.x, p.y + 10.5, q1.z), vec3(1, 1, 1)), vec3(0.3, 0.3, 0.3));
	//floor = disMin(floor, vec4(sdBox(vec3(q1.x, p.y - 13.5, q1.z), vec3(1, 1, 1)), vec3(0.3, 0.3, 0.3)));

	float c = 25;
	vec3 q = vec3(mod(p.x, c)- 0.5*c, p.y, p.z);

	vec4 table = vec4(sdBox(p - vec3(0, -7, 22), vec3(6, 3, 5)), vec3(0.2, 0.2, 0.2));
	table = sun(table, vec4(sdBox(p - vec3(0, -7, 22), vec3(4, 4, 5.3)), vec3(0.2, 0.2, 0.2)));
	vec4 pillars = vec4(sdBox(q - vec3(0, -9, 22), vec3(0.5, 22, 0.5)), vec3(0.2, 0.2, 0.2));
	vec4 disRoom = sun(floor, sun(pillars, sun(table, box)));
	return disMin(symbol(p), disRoom);
}

vec3 getNormal(vec3 p, vec3 rd) {
	vec3 ep = vec3(0.01, 0, 0);
    vec3 normal;
    normal.x = map(p + ep.xyz, rd).x - map(p - ep.xyz, rd).x;
    normal.y = map(p + ep.yxz, rd).x - map(p - ep.yxz, rd).x;
    normal.z = map(p + ep.yzx, rd).x - map(p - ep.yzx, rd).x;
    return normalize(normal);
}

void main() {
    float u = fragCoord.x * 2.0 - 1.0;
	float v = fragCoord.y * 2.0 - 1.0;
	float k = 1;// + 1*posSin(5*cos(iTime));
	u *= 16.0 / 9.0;
	u *= k;
	v *= k;
    vec3 ro = vec3(0,  -1+0.55*cos(iTime), -15.0);
    vec3 tar = vec3(0, 0, 0);//vec3(0.2*cos(20*iTime), 0.2*cos(10*iTime), 0.2*cos(10*iTime));
    vec3 dir = normalize(tar - ro);
	vec3 right = normalize(cross(vec3(0.0, 1.0, 0.0), dir));
	vec3 up = cross(dir, right);
	vec3 rd = normalize(dir + right*u + up*v);
                    
                    
    float t = 0.0;
    vec3 color = vec3(0.0);
	float transmittance = 1.0;
	vec3 scatteredLight = vec3(0.0);

    
    for (int i = 0; i < 200; i++) {
    	vec3 p = ro + rd * t;
        
        vec4 disColor = map(p, rd);
		float d = disColor.x;
		color = disColor.yzw;
        float fogAmount = 0.005;
		vec4 lightDisColor = symbol(p);
		float lol = lightDisColor.x;
		vec3 light = 40.0 * lightDisColor.yzw / (lol*lol);
		//vec4 lightColDis = vec4(1, 0, 0, );
		//vec3 light = lightColDis.rgb;

		vec3 lightIntegrated = light - light * exp(-fogAmount * d);
		scatteredLight += transmittance * lightIntegrated;	
		transmittance *= exp(-fogAmount * d);
        
		if (d < 0.01) {
           // color = vec3(0.6, 0.2, 0.1);
         
            float dis = symbol(p).x;
            vec3 invLight = normalize(lightPosition - p);
            
            vec3 normal = getNormal(p, rd);
            
            float diffuse = max(0.0, dot(invLight, normal));
            float s = 10.0;
            float k = max(0.0, dot(rd, reflect(invLight, normal)));
            float spec =  pow(k, s);
            float str = 12.0/(0.01 + 0.4*dis + 0.03*dis*dis);
            color *= (0.4 + 0.5*diffuse);
		//	color += spec;
			//float lavaDistance = length(river(p));

			//float lavaLight = 3 / (1 + 1 * lavaDistance + 500*lavaDistance * lavaDistance * lavaDistance);
			//color += vec3(min(0.1, lavaLight));

			color = transmittance * color + scatteredLight;
			color *= str;
            break;
        }
        t += 0.5*d;
    }   
	float time = mod(iTime, 40);
	float fstr = smoothstep(0, 2, time);
	float fstr2 = smoothstep(30, 40, time);
	fragColor = vec4(color, 1.0);
    fragColor.rgb = fragColor.rgb / (fragColor.rgb + vec3(1.0)) * (fstr - fstr2);
}

)""  