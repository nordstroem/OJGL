R""(
#version 430

in vec2 fragCoord;
out vec4 fragColor;

uniform float iGlobalTime;
uniform sampler2D inTexture0;
uniform sampler2D inTexture1;
uniform sampler2D inTexture2;

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

#define REFLECTION
//#define REFRACTION // TODO: I don't think this works perfectly.

vec2 un(vec2 a, vec2 b)
{
	return a.x < b.x ? a : b;
}

float sdBox( vec3 p, vec3 b )
{
  vec3 d = abs(p) - b;
  return min(max(d.x,max(d.y,d.z)),0.0) + length(max(d,0.0));
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

//float sdHexPrism( vec3 p, vec2 h )
//{
//    vec3 q = abs(p);
//    return max(q.z-h.y,max((q.x*0.866025+q.y*0.5),q.y)-h.x);
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










vec4 light(vec3 p) {

//	float s = 30.0;
//	p.xz = mod(p.xz + vec2(s*0.5), s) - s * 0.5;
//	float t = iGlobalTime;
	vec3 lightCol  = vec3(1);
			lightCol = normalize(lightCol);
	vec3 lightPos = vec3(0);
	vec3 L = lightPos-p;


	float dis = 99999;
	float t = iGlobalTime;
//	if (t >= 6.5) {
		float l2t = clamp((iGlobalTime - 6.6) * 2.0, 0, 1);
		float leg2 = sdCappedCylinder(p - vec3(0, -3.0 + 1.5*l2t, -0.5), vec2(0.01, 1.5 * l2t));
		dis = min(dis, leg2 + 1 -  step(6.6, iGlobalTime));
//	}
//	if (t >= 7.0) {
		float ct = clamp((iGlobalTime - 7.1) * 3.0, 0, 1);
		float cyl1 = sdCappedCylinder(p.xzy - vec3(0, -0.5 + 0.5 * ct, 0), vec2(0.01, 0.5 * ct));
		dis = min(dis, cyl1 + 1 -  step(7.1, iGlobalTime));
//	}
//	if (t >= 7.333) {
		float l1t = clamp((iGlobalTime - 7.433) * 2.0, 0, 1);
		float leg1 = sdCappedCylinder(p - vec3(0,  0.0 - 1.5*l1t, 0.5), vec2(0.01, 1.5*l1t));
		dis = min(dis, leg1 + 1 -  step(7.433, iGlobalTime));
//	}
	vec3 r = p;
	float w = 1.5 * smoothstep(0.0, 15.0 ,abs(p.z));
	r.x -= + sin(p.z*0.25) * w;
	r.x -= + sin(p.z*0.15) * w;
	r.y -= -3.0;
	r.z -= -50;

	if (t < 7.93) {
		t = min(t, 6.65);
	} else {
		t -= 1.2;
	}
	float rt = t * 6;
	float river = sdCappedCylinder(r.xzy, vec2(0.01, 10.0 + rt));
	river = max(-sdBox(p - vec3(0, -3.0, 0), vec3(0.5)), river);
//	if (abs(p.z) < 0.6) {
////		river = 999;
//		river += abs(p.z) + 0.3;
//	}

	dis = 	min(dis, river);//min(min(leg1, leg2), min(cyl1, river));
//	float distanceToL = max(0.0001, dis);
//	vec3 Lnorm = L/distanceToL;

	vec3 point = vec3(1,0.55,1) * 5.0/(dis*dis);


//	return vec4(point * (0.1 + 1.0*drum) + points, min(dis, diss));
	float drum = 1337.0;
	return vec4(point * (0.1 + 1.0*drum), dis);
}

vec4 lights(vec3 p) {
	vec3 r = p;
	float w = 1.5 * smoothstep(0.0, 15.0 ,abs(p.z));
	r.x -= + sin(p.z*0.25) * w;
	r.x -= + sin(p.z*0.15) * w;
	r.y -= -3.0;
	r.z -= -50;

	float s = 5.0;
	r.z = mod(p.z, s) - s * 0.5;
	r.y -=  0.7;
//	s = 1.0;
//	r.y = mod(r.y, s) - s * 0.5;
	float sw = 3.0  - 1.5 * smoothstep(0.0, 6.0 ,abs(p.z));
//	r.x -= sw * 1.1;
	r.x = abs(r.x) - sw * 1.3;
	float diss = length(r) - 0.0;
	vec3 col = vec3(0.5,0.5,1);
	vec3 points = col * 10.0/(diss*diss);
	return vec4(points, diss);
}

vec3 lightAPos(vec3 p)
{
	p.z = mod(p.z, 4.0) - 2.0;
	return p;
}

vec4 lightA(vec3 p)
{
	vec3 lightPos = vec3(2, 0, 0);
	p = lightAPos(p);
	float dis = length(p - lightPos);
	vec3 col = vec3(1.0, 0.0, 0.0);
	const float strength = 10.0;
	vec3 res = col * strength / (dis * dis * dis);
	return vec4(res, dis);
}

vec3 lightBPos(vec3 p)
{
	return p - vec3(0.0, 2.0 * sin(iGlobalTime), 0.0);
}

vec4 lightB(vec3 p)
{
	vec3 lightPos = vec3(-1, 0, 0);
	p = lightBPos(p);
	float dis = length(p - lightPos);
	vec3 col = vec3(0.0, 0.0, 1.0);
	const float strength = 10.0;
	vec3 res = col * strength / (dis * dis * dis);
	return vec4(res, dis);
}

vec4 lightUnion(vec4 a, vec4 b)
{
	return vec4(a.rgb + b.rgb, min(a.w, b.w));
}

vec4 evaluateLight(vec3 pos)
{
	vec4 res = lightA(pos);
	res = lightUnion(res, lightB(pos));
	return res;
}

void addLight(inout vec3 diffRes, inout float specRes, vec3 normal, vec3 eye, vec3 pos, vec3 lightCol) 
{
	vec3 col = vec3(0.0);
	vec3 invLight = normalize(-pos);
	float diffuse = max(0.0, dot(invLight, normal));
	float spec = specular(normal, -invLight, normalize(eye - pos), 50.0);
	float dis = length(-pos);
	float str = 1.0/(0.5 + 0.01*dis + 0.1*dis*dis); 

	diffRes = diffuse * lightCol;
	specRes = spec * str;
}

void addLightning(inout vec3 color, vec3 normal, vec3 eye, vec3 pos) {
	vec3 diffuse = vec3(0.0);
	float specular = 0.0;
	// TODO kan lightA ta pos som argument så man inte upprepar`?
	addLight(diffuse, specular, normal, eye, lightAPos(pos), lightA(pos));
	//addLight

	color = color * diffuse + specular;

	vec3 r = pos;
	float w = 1.5 * smoothstep(0.0, 15.0 ,abs(pos.z));
	r.x -= + sin(pos.z*0.25) * w;
	r.x -= + sin(pos.z*0.15) * w;
	r.y -= -3.0;
	r.z -= -50;
	float s = 5.0;
	r.z = mod(pos.z, s) - s * 0.5;
	r.y -=  0.7;
	float sw = 3.0  - 1.5 * smoothstep(0.0, 6.0 ,abs(pos.z));
	r.x = abs(r.x) - sw * 1.3;
	vec3 slpos = r; 
	vec3 invLights = normalize(-slpos); //////
	float diffuses = max(0.0, dot(invLights, normal)); //////
	float specs = specular(normal, -invLights, normalize(eye - slpos), 50.0); //////
	float diss = length(-slpos); //////
	float strs = 1.0/(0.5 + 0.01*diss + 0.1*diss*diss); //////

	vec3 lpos = vec3(0,-1,1);

	float dis = length(lpos - pos);
	vec3 invLight = normalize(lpos - pos);
	float diffuse = max(0.0, dot(invLight, normal));
	float spec = specular(normal, -invLight, normalize(eye - pos), 220.0);

	float str = 1.0/(0.5 + 0.01*dis + 0.1*dis*dis);
	//color =  color * (0.05 + 0.9*diffuse*light(pos).xyz + 0.2 * diffuses * lights(pos).xyz ) + spec*str + specs*strs*0.5;
	color = color * (0.05 + 0.2*diffuses*lights(pos).xyz) + specs*strs*0.5;
	color = clamp(color, vec3(0), vec3(1));
}

#define MAT_GROUND 1.0
#define MAT_PEND 2.0
#define MAT_WATER 3.0

// TODO: cleanup and fix noise texture
vec2 water(vec3 p, vec3 rd)
{
	if (rd.y > 0) {
		return vec2(99999, MAT_WATER);
	}
	p -= - 3.5;
	float t = iGlobalTime * 1.5;

	float d = (sin(p.x + t*0.5) + sin(p.z  + t*0.5)) * 0.1 +
			length(texture(inTexture0, p.xz*0.8 + vec2(0, t*0.1)))*0.1 +
			length(texture(inTexture0, p.xz*0.8 + vec2(t*0.13, 0)))*0.1;
	d *= 0.5;

	float h = p.y - d * 0.1;

	float dis = (0.1 -p.y)/rd.y;

	return vec2(max(h, dis), MAT_WATER);
}

vec2 map(vec3 p, vec3 rd) 
{
	vec2 res = vec2(p.y + 3.0, MAT_GROUND);
	res = un(res, vec2(sdBox(p - vec3(0,-0.5, 0), vec3(0.5)), MAT_GROUND));
	res = un(res, vec2(sdBox(p - vec3(0, 1.0, 0), vec3(0.3)), MAT_GROUND));
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


vec3 raymarch(vec3 ro, vec3 rd, vec3 eye) 
{
	const int maxIter = 90;
	const float maxDis = 200.0;
	const int jumps = 3;

	vec3 col = vec3(0);	
	float ref = 1.0;

	vec3 scatteredLight = vec3(0.0);
	float transmittance = 1.0;
	for (int j = 0; j < jumps; j++) {
		float t = 0.0;
		for (int i = 0; i < maxIter; i++) {
			vec3 p = ro + rd * t;
			vec2 res = map(p, rd);
			float d = res.x;
			float m = res.y;

			float fogAmount = 0.005;
			vec4 lightColDis = evaluateLight(p);
			vec3 light = lightColDis.rgb;
			d = min(d, lightColDis.w);

			vec3 lightIntegrated = light - light * exp(-fogAmount * d);
			scatteredLight += transmittance * lightIntegrated;	
			transmittance *= exp(-fogAmount * d);

			t += d;		
			bool end = i == maxIter - 1 || t > maxDis;
			if (d < 0.01 || end) {
				vec3 c = vec3(1, 0, 1);
				vec3 normal = getNormal(p, rd);

				if (m == MAT_GROUND) {
					c = mix(vec3(0.3,0.9,0.3), vec3(0.9,0.8,0.3), min(normal.y * 1.0, 1));
				} else if (m == MAT_PEND) {
					c = vec3(1, 0.5, 1);
				} else if (m == MAT_WATER) {
					c = vec3(0, 0, 0);
				}

				c *= occlusion(p, normal, rd);
				addLightning(c, normal, eye, p);
				if (end) {
					transmittance = 0;
				}
				col = mix(col, transmittance * c + scatteredLight, ref);

				if (m == MAT_GROUND) {
					return col;
				} else if (m == MAT_PEND) {
					ref *= abs(normal.z);
				} else {
					ref *= 0.9;
				}

#ifdef REFLECTION
				rd = reflect(rd, getNormal(p, rd));
#endif
#ifdef REFRACTION
				rd = refract(rd, getNormal(p, rd), 1/1.2);
#endif
				ro = p + rd*0.5;
				t = 0;
				break;
			}
		}
	}
	return col;
}

void main()
{
    float u = fragCoord.x * 2.0 - 1.0;
	float v = fragCoord.y * 2.0 - 1.0;

    vec3 eye = vec3(2 * sin(iGlobalTime), 1, 2 * cos(iGlobalTime));
	vec3 tar = vec3(0 ,0, 0); 

	vec3 dir = normalize(tar - eye);
	vec3 right = normalize(cross(vec3(0, 1, 0), dir));
	vec3 up = cross(dir, right);

	vec3 ro = eye;
	vec3 rd = normalize(dir + right*u + up*v);
	
	vec3 color = raymarch(ro, rd, eye);
	color /= (color + vec3(1.0));
    fragColor = vec4(color, 1.0);  
} 

)""  