R"(
#version 430

in vec2 fragCoord;
out vec4 fragColor;

uniform float iGlobalTime;

uniform float CHANNEL_0_TIME_SINCE[41];
uniform float CHANNEL_1_TIME_SINCE[23];
uniform float CHANNEL_11_TIME_SINCE[28];
uniform float CHANNEL_12_TIME_SINCE[2];
uniform float CHANNEL_13_TIME_SINCE[1];

uniform float CHANNEL_0_TIME_TO[41];
uniform float CHANNEL_1_TIME_TO[23];
uniform float CHANNEL_11_TIME_TO[28];
uniform float CHANNEL_12_TIME_TO[2];
uniform float CHANNEL_13_TIME_TO[1];

uniform float CHANNEL_12_TOTAL;
uniform float CHANNEL_13_TOTAL;

//////////////////////////////////////////////////////


vec2 un(vec2 a, vec2 b)
{
	return a.x < b.x ? a : b;
}

float sdBox( vec3 p, vec3 b )
{
  vec3 d = abs(p) - b;
  return min(max(d.x,max(d.y,d.z)),0.0) + length(max(d,0.0));
}

float sdHexPrism( vec3 p, vec2 h )
{
    vec3 q = abs(p);
    return max(q.z-h.y,max((q.x*0.866025+q.y*0.5),q.y)-h.x);
}

//float udBox( vec3 p, vec3 b )
//{
//  return length(max(abs(p)-b,0.0));
//}

float sdCylinder( vec3 p, float r )
{
  return length(p.xz)-r;
}
//
//vec4 texCube( sampler2D sam, in vec3 p, in vec3 n, in float k )
//{
//	vec4 x = texture2D( sam, p.yz );
//	vec4 y = texture2D( sam, p.zx );
//	vec4 z = texture2D( sam, p.xy );
//    vec3 w = pow( abs(n), vec3(k) );
//	return (x*w.x + y*w.y + z*w.z) / (w.x+w.y+w.z);
//}


float udRoundBox( vec3 p, vec3 b, float r )
{
  return length(max(abs(p)-b,0.0))-r;
}

float sdCappedCylinder( vec3 p, vec2 h )
{
  vec2 d = abs(vec2(length(p.xz),p.y)) - h;
  return min(max(d.x,d.y),0.0) + length(max(d,0.0));
}
//
//float sdTorus( vec3 p, vec2 t )
//{
//  vec2 q = vec2(length(p.xz)-t.x,p.y);
//  return length(q)-t.y;
//}

float specular(vec3 normal, vec3 light, vec3 viewdir, float s)
{
	float nrm = (s + 8.0) / (3.1415 * 8.0);
	float k = max(0.0, dot(viewdir, reflect(light, normal)));
    return  pow(k, s);
}

float smin( float a, float b)
{
	float k = 0.1;
    float h = clamp( 0.5+0.5*(b-a)/k, 0.0, 1.0 );
    return mix( b, a, h ) - k*h*(1.0-h);
}

//float smink( float a, float b, float k )
//{
//    float h = clamp( 0.5+0.5*(b-a)/k, 0.0, 1.0 );
//    return mix( b, a, h ) - k*h*(1.0-h);
//}
//vec2 sun(vec2 a, vec2 b)
//{
//	float sm = smin(a.x,b.x);
//	float ca = abs(sm -a.x);
//	float cb = abs(sm -b.x);
//
//	return ca < cb ? vec2(sm, a.y) : vec2(sm, b.y);
//}
//
//vec2 sunk(vec2 a, vec2 b, float k)
//{
//	float sm = smink(a.x,b.x, k);
//	float m = min(a.x, b.x);
//	float ca = abs(sm -a.x);
//	float cb = abs(sm -b.x);
//	return ca < cb ? vec2(sm, a.y) : vec2(m, b.y);
//}



#define FL_POS vec3(0, -3.5, 15.0)

vec4 floorLight(vec3 p) {
	vec3 o = p;
	float s = 0.3;
	p.x = mod(p.x, s) - s * 0.5;
	float diss = sdCylinder(p.xzy - vec3(0, -3.0, 0).xzy, 0.00);
	vec3 col = vec3(0.5,0.5,1);
	float music = 0;
	int index = int(o.x / s) + 15;
	if (index >= 0 && index < 28) {
		music = pow(0.99, CHANNEL_11_TIME_SINCE[index]);
	}
	vec3 points = col * 1.0  * music / (diss);
	return vec4(points, diss);
}

vec3 evaluateLight(vec3 pos, inout float dis)
{
	//vec4 l = light(pos);
	//vec4 sl = lights(pos);
	//dis = min(l.w, sl.w);
	//return l.xyz + sl.xyz;
	vec4 fl = floorLight(pos);
	dis = fl.w;
	return fl.xyz;

}

void addLightning(inout vec3 color, vec3 normal, vec3 eye, vec3 pos) {
	
	vec3 invLights = normalize(-FL_POS);
	float diffuses = max(0.0, dot(invLights, normal));
	float specs = specular(normal, -invLights, normalize(eye - FL_POS), 50.0);
	float diss = floorLight(pos).w;//length(-FL_POS);
	float strs = 1.0/(0.5 + 0.01*diss + 0.1*diss*diss);

	vec3 lpos = vec3(0,-1,1);

	//float dis = length(lpos - pos);
	//vec3 invLight = normalize(lpos - pos);
	//float diffuse = max(0.0, dot(invLight, normal));
	//float spec = specular(normal, -invLight, normalize(eye - pos), 220.0);

	//float str = 1.0/(0.5 + 0.01*dis + 0.1*dis*dis);
	float tmp = 0;
	//color =  color * (0.05 + 0.9*diffuse*light(pos).xyz + 0.2 * diffuses * floorLight(pos).xyz ) + spec*str + specs*strs*0.5;
	color =  color * (0.05 + 0.2 * diffuses * floorLight(pos).xyz ) + specs*strs*0.5;
	color = clamp(color, vec3(0), vec3(1));

}







#define TEX 0.2
#define SIZE 8.0
#define PI 3.1415

#define MAT_GROUND 2.0
#define MAT_PEND 3.0
#define MAT_WATER 5.0
#define MAT_TUNNEL 6.0
#define MAT_THRESHOLD 7.0








vec2 tunnel(vec3 p ) {
	float d = -sdHexPrism(p, vec2(4.0, 99999.0));
	return vec2(d, MAT_TUNNEL);
}

vec2 threshold(vec3 p ) {
	float d = sdHexPrism(p - vec3(0, 2.5, 3), vec2(1.0, 0.5));
	return vec2(d, MAT_THRESHOLD);
}

vec2 map(vec3 p, vec3 rd) {
	vec2 res = tunnel(p);
	res = un(res, threshold(p));
	return res;
}



vec3 getNormal(vec3 p, vec3 rd)
{
	vec3 normal;
    vec3 ep = vec3(0.01, 0, 0);
    normal.x = map(p + ep.xyz, rd).x - map(p - ep.xyz, rd).x;
    normal.y = map(p + ep.yxz, rd).x - map(p - ep.yxz, rd).x;
    normal.z = map(p + ep.yzx, rd).x - map(p - ep.yzx, rd).x;
    return normalize(normal);

}

float occlusion(vec3 p, vec3 normal, vec3 rd)
{
	float o = clamp(2*map(p + normal * 0.5, rd).x, 0, 1);
	return 0.2 + 0.8*o;
}



vec3 raymarch(vec3 ro, vec3 rd, vec3 eye) {
	float t = 0.0;
	int maxIter = 200;
	const float maxDis = 200.0;
	float d = 0.0;
	vec3 p = vec3(-1.0, -1.0, -1.0);
	vec3 col = vec3(0);
	const int jumps = 3;
	float ref = 1.0;
	vec3 scatteredLight = vec3(0.0);
	float transmittance = 1.0;
//	for (int j = 0; j < jumps; j++) {
		for (int i = 0; i < maxIter; i++) {
			p = ro + rd * t;

			vec2 res = map(p, rd);
			d = res.x;
//			d = min(d, 0.2);
			float fogAmount = 0.01;
			float lightDis = -1.0;
			vec3 light = evaluateLight(p, lightDis);
//			d = min(d, lightDis);
			d = min(min(d, 1), max(lightDis*0.5, 0.05));
			vec3 lightIntegrated = light - light * exp(-fogAmount * d);
			scatteredLight += transmittance * lightIntegrated;
			transmittance *= exp(-fogAmount * d);

			t += d;
			float m = res.y;
			bool end = i == maxIter - 1 ||t > maxDis;
			if (d < 0.01 || end) {
				vec3 c = vec3(1);
				vec3 normal = getNormal(p, rd);
				if (m == MAT_GROUND) {
//					c = mix(vec3(0.9,0.8,0.3), vec3(0.9,0.2,0.5), min(normal.y * 1.5, 1));
					c = mix(vec3(0.3,0.9,0.3), vec3(0.9,0.8,0.3), min(normal.y * 1.0, 1));
				} else if (m == MAT_PEND) {
					c = vec3(1, 0.5, 1);
				} else if (m == MAT_WATER) {
					c = vec3(0, 0, 0);
				} else if (m == MAT_TUNNEL) {
					c = vec3(1, 0, 0);
				} else if (m == MAT_THRESHOLD) {
					c = vec3(0, 1, 0);
				} 
				c *= occlusion(p, normal, rd);
				addLightning(c, normal, eye, p);
				if (end) {
					transmittance = 0;
				}
				col = mix(col, transmittance * c + scatteredLight, ref);
//				col = mix(col, vec3(0.7), clamp(t * 0.01, 0, 1));
				if (m == MAT_GROUND) {
					return col;
				} else if (m == MAT_PEND) {
//					ref *= 0.9;
					ref *= abs(normal.z);
				} else {
					ref *= 0.9;
				}
//				ref = 0.0;
				rd = reflect(rd, getNormal(p, rd));
				ro = p + rd*0.05;
				t = 0;
				i = 0;
				maxIter = 0;
			}
//			if (t > maxDis) {
//				return col;
//			}
		}

//		if (ref < 0.1) {
//			break;
//		}
//	}
	return col;
}





void main()
{
	float u = fragCoord.x * 2.0 - 1.0;
	float v = fragCoord.y * 2.0 - 1.0;
    
	float t = iGlobalTime * 0.5;
    vec3 eye = vec3(0, 0, iGlobalTime);
	vec3 tar = eye + vec3(0,0, 1); 

	float r = 3 + 0.5 * sin(iGlobalTime);

	vec3 dir = normalize(tar - eye);
	vec3 right = normalize(cross(vec3(0, 1, 0), dir));
	vec3 up = cross(dir, right);

	vec3 ro = eye;
	vec3 rd = normalize(dir + right*u + up*v);

	vec3 light = vec3(0.0, 0.0, 26.0 );

	float material = -1.0;
	vec3 color = raymarch(ro, rd, eye);
    fragColor = vec4(color, 1.0);
    fragColor.rgb = fragColor.rgb / (fragColor.rgb + vec3(1.0));
}

)"   