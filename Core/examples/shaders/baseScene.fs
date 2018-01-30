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


vec3 evaluateLight(vec3 pos, inout float dis)
{
	vec4 l = light(pos); 
	vec4 sl = lights(pos); dis = sl.w; return sl.xyz;
	dis = min(l.w, sl.w);
	return l.xyz + sl.xyz;
//	return l.xyz;
}

void addLightning(inout vec3 color, vec3 normal, vec3 eye, vec3 pos) {
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
	//dis += step(0, -abs(p.x) + s) * (s - abs(p.x));
//	r.x -= sw * 1.1;
	r.x = abs(r.x) - sw * 1.3;
	vec3 slpos = r;
	vec3 invLights = normalize(-slpos);
	float diffuses = max(0.0, dot(invLights, normal));
	float specs = specular(normal, -invLights, normalize(eye - slpos), 50.0);
	float diss = length(-slpos);
	float strs = 1.0/(0.5 + 0.01*diss + 0.1*diss*diss);

	vec3 lpos = vec3(0,-1,1);//LPOS;//vec3(1, 1, -2);

	float dis = length(lpos - pos);
	vec3 invLight = normalize(lpos - pos);
	float diffuse = max(0.0, dot(invLight, normal));
	float spec = specular(normal, -invLight, normalize(eye - pos), 220.0);

	float str = 1.0/(0.5 + 0.01*dis + 0.1*dis*dis);
	//str = towerLight(pos).x;
	float tmp = 0;
//	color =  color * (0.05 + 0.9*diffuse*light(pos).xyz) + spec*str;
	//color =  color * (0.05 + 0.9*diffuse*light(pos).xyz + 0.2 * diffuses * lights(pos).xyz ) + spec*str + specs*strs*0.5;
	color = color * (0.05 + 0.2*diffuses*lights(pos).xyz) + specs*strs*0.5;
	color = clamp(color, vec3(0), vec3(1));
	//color *= str;

}



//mat3 rot(float x, float y, float z)
//{
//	float cx = cos(x);
//	float sx = sin(x);
//	float cy = cos(y);
//	float sy = sin(y);
//	float cz = cos(z);
//	float sz = sin(z);
//	mat3 xm = mat3(1, 0, 0,
//					0, cx, -sx,
//					0, sx, cx);
//	mat3 ym = mat3(cy, 0, sy,
//			  		0, 1, 0,
//			  		-sy, 0, cy);
//	mat3 zm = mat3(cz, -sz, 0,
//					sz, cz, 0,
//					0, 0, 1);
//	return xm * ym * zm;
//}

mat3 rot(float z)
{
	float cx = 1.0;
	float sx = 0.0;
	float cy = 1.0;
	float sy = 0.0;
	float cz = cos(z);
	float sz = sin(z);
//	mat3 xm = mat3(1, 0, 0,
//					0, cx, -sx,
//					0, sx, cx);
//	mat3 ym = mat3(cy, 0, sy,
//			  		0, 1, 0,
//			  		-sy, 0, cy);
	mat3 zm = mat3(cz, -sz, 0,
					sz, cz, 0,
					0, 0, 1);
//	return xm * ym * zm;
	return zm;
}

#define TEX 0.2
#define SIZE 8.0
#define PI 3.1415

#define MAT_GROUND 2.0
#define MAT_PEND 3.0
#define MAT_WATER 5.0









//vec2 tree(vec3 p) {
//	float a  = atan(p.z, p.x);
//	float r = length(p.xz);
//	//r = mod(r,5) - 0.5;
////	a = mod(a + iGlobalTime, 1) - 0.5;
//	vec3 pn = vec3(r*cos(a), p.y, r*sin(a));
////	float dis = udRoundBox(pn, vec3(1), 0.5);
////	float dis = sdCylinder(pn,  0.2);
//	float s = 2.5;
//	float rr = sin(p.y);
//	float dis = sdCylinder(p - vec3(rr*cos(p.y * s - 4*iGlobalTime), 0, rr*sin(p.y * s - 4*iGlobalTime)),  0.15);
//	float dis2 = sdCylinder(p - vec3(-rr*cos(p.y * s - 4*iGlobalTime), 0, -rr*sin(p.y * s - 4*iGlobalTime)),  0.15);
//	rr *= 1.9 ;
//	float dis3 = sdCylinder(p - vec3(rr*cos(p.y * s - 4*iGlobalTime), 0, rr*sin(p.y * s - 4*iGlobalTime)),  0.1);
//	float dis4 = sdCylinder(p - vec3(-rr*cos(p.y * s - 4*iGlobalTime), 0, -rr*sin(p.y * s - 4*iGlobalTime)),  0.1);
//	dis = smin(dis, dis2);
////	dis3 = smin(dis3, dis4);
//	dis = smin(dis, dis3);
//	float sp = length(p - vec3(0, -2, 0)) - 2;
//	dis = smin(sp, dis);
//	return vec2(0.2*dis, MAT_TREE);
//}

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

vec2 pground(vec3 p, vec3 rd)  {
	float incDrum = 1337;
	float drum = 1337.0;
	p.y -= -2.8;
	float d = (incDrum + 1 - drum);
	float dis = p.y + 0.5*texture(inTexture0, p.xz*0.25 ).x;
	float w = 1.5 * smoothstep(0.0, 15.0 ,abs(p.z));
	p.x -= + sin(p.z*0.25) * w;
	p.x -= + sin(p.z*0.15) * w;
	float s = 3.0  - 1.5 * smoothstep(0.0, 6.0 ,abs(p.z));
//	if (abs(p.x) < s) {
		dis += step(0, -abs(p.x) + s) * (s - abs(p.x));
//	}

	return vec2(dis*0.5, MAT_GROUND);
}

vec2 pendulum(vec3 p) {
float theta1 = 1337.0;
float theta2 = 1337.0;
	float c1x = 1337.0;
	float c1y = 1337.0;
	float c2x = 1337.0;
	float c2y = 1337.0;
	vec3 p1 = (p - vec3(c1x, c1y, 0)) * rot(-theta1);
	vec3 p2 = (p - vec3(c2x, c2y, 0)) * rot(-theta2);
	vec3 c =  p -  vec3(c1x * 2.0, c1y * 2.0, 0);
	float c1a = udRoundBox(p1 - vec3(0, 0, -0.07), vec3(0.15, 0.6, 0.01), 0.02);
	float c1b = udRoundBox(p1 - vec3(0, 0, 0.07), vec3(0.15, 0.6, 0.01), 0.02);
	float c1 = min(c1a, c1b);
//	float c1 = udRoundBox(p1, vec3(0.15, 0.6, 0.01), 0.01);
	float c2 = udRoundBox(p2, vec3(0.15, 0.45, 0.01), 0.02);
	float dis = min(c1, c2);
	vec2 res = vec2(dis  + 1 - step(9.5, iGlobalTime), MAT_PEND);
	return res;
}

vec2 map(vec3 p, vec3 rd) {
	vec2 res = pground(p, rd);
//	if (iGlobalTime > 9.5) {
		res = un(res, pendulum(p));
//	}
	res = un(res, water(p, rd));
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
	int maxIter = 90;
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
			float fogAmount = 0.005;// + max(0, -p.y * 0.5);
//			fogAmount += smoothstep(28*7, 29*7,p.z) * 5.0;
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
				vec3 c = vec3(1, 0, 1);
				vec3 normal = getNormal(p, rd);
				if (m == MAT_GROUND) {
//					c = mix(vec3(0.9,0.8,0.3), vec3(0.9,0.2,0.5), min(normal.y * 1.5, 1));
					c = mix(vec3(0.3,0.9,0.3), vec3(0.9,0.8,0.3), min(normal.y * 1.0, 1));
					//c = vec3(1, 0, 0);
				} else if (m == MAT_PEND) {
					c = vec3(1, 0.5, 1);
				} else if (m == MAT_WATER) {
					c = vec3(0, 0, 0);
				}
				//c *= occlusion(p, normal, rd);
				addLightning(c, normal, eye, p);
				if (end) {
					transmittance = 0;
				}
				col = mix(col, transmittance * c + scatteredLight, ref);
				//col = c;
				//return col;
//				col = mix(col, vec3(0.7), clamp(t * 0.01, 0, 1));
				if (m == MAT_GROUND) {
					return col;
					//return vec3(1.0, 0.0, 0.0);
				} else if (m == MAT_PEND) {
//					ref *= 0.9;
					ref *= abs(normal.z);
				} else {
					ref *= 0.9;
				}
//				ref = 0.0;
				rd = reflect(rd, getNormal(p, rd));
//				rd = refract(rd, getNormal(p, rd), 1/1.2);
				ro = p + rd*0.05;
				t = 0;
				i = 0;
				maxIter /= 2;
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
	//float u = (fragCoord.x / iResolution.x) * 2.0 - 1.0; // 
    //float v = ((fragCoord.y / iResolution.y) * 2.0 - 1.0)  * (9.0/ 16.0);//* (iResolution.y/iResolution.x);
    float u = fragCoord.x * 2.0 - 1.0;
	float v = fragCoord.y * 2.0 - 1.0;

	float t = iGlobalTime * 0.5;
    vec3 eye = vec3(2* sin(t), 2 , 2 * cos(t));
	vec3 tar = vec3(0,0, 0); //eye + vec3(0.0, 0.0, 1.0);
//	if (iGlobalTime < 10.0) {
//		eye = vec3(3, 1, iGlobalTime * 4 - 40);
//		tar = eye + vec3(-0.5, -0.4, 1);
//	}
	float r = 3 + 0.5 * sin(iGlobalTime);
	eye = mix(vec3(3, 1.5, iGlobalTime * 4 - 40), vec3(r* sin(-t), 2 + sin(iGlobalTime) * 0.5 , r * cos(-t)), smoothstep(9.5,11.0,iGlobalTime));
	tar = mix(eye + vec3(-0.5, -0.8, 1), vec3(0,0, 0), smoothstep(9.5,11.0,iGlobalTime));
	if (iGlobalTime > 20.0) {
		float c2x = 1337.0;
		float c2y = 1337.0;
		eye = vec3(c2x,c2y,1.5);
		tar = eye + vec3(0,0,-1);
	}
	vec3 dir = normalize(tar - eye);
	vec3 right = normalize(cross(vec3(0, 1, 0), dir));
	vec3 up = cross(dir, right);

	vec3 ro = eye;
	vec3 rd = normalize(dir + right*u + up*v);

	vec3 light = vec3(0.0, 0.0, 26.0 );

	float material = -1.0;
	vec3 color = raymarch(ro, rd, eye);
    fragColor = vec4(color, 1.0);
    //fragColor.rgb = fragColor.rgb / (fragColor.rgb + vec3(1.0));
    //fragColor.rgb = mix(fragColor.rgb, vec3(1), smoothstep(25.5, 26.0, iGlobalTime));

	//fragColor.rgb = vec3(rd.z, 0.0, 0.0);
	//fragColor.rgb = vec3(1.0, 0.0, 0.0);
} 


/*
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
}*/

)""  