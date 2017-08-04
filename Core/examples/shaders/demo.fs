R"(
#version 430

in vec2 fragCoord;
out vec4 fragColor;

uniform float iGlobalTime;

//////////////////////////////////////////////////////
vec2 un(vec2 a, vec2 b)
{
	return a.x < b.x ? a : b;
}



vec4 texCube( sampler2D sam, in vec3 p, in vec3 n, in float k )
{
	vec4 x = texture2D( sam, p.yz );
	vec4 y = texture2D( sam, p.zx );
	vec4 z = texture2D( sam, p.xy );
    vec3 w = pow( abs(n), vec3(k) );
	return (x*w.x + y*w.y + z*w.z) / (w.x+w.y+w.z);
}

float sdBox( vec3 p, vec3 b )
{
  vec3 d = abs(p) - b;
  return min(max(d.x,max(d.y,d.z)),0.0) + length(max(d,0.0));
}

float udRoundBox( vec3 p, vec3 b, float r )
{
  return length(max(abs(p)-b,0.0))-r;
}

float sdCappedCylinder( vec3 p, vec2 h )
{
  vec2 d = abs(vec2(length(p.xz),p.y)) - h;
  return min(max(d.x,d.y),0.0) + length(max(d,0.0));
}

float sdTorus( vec3 p, vec2 t )
{
  vec2 q = vec2(length(p.xz)-t.x,p.y);
  return length(q)-t.y;
}

float length8( vec2 p )
{
	p = p*p; p = p*p; p = p*p;
	return pow( p.x + p.y, 1.0/8.0 );
}

float sdTorus88( vec3 p, vec2 t )
{
  vec2 q = vec2(length8(p.xz)-t.x,p.y);
  return length8(q)-t.y;
}

float sdHexPrism( vec3 p, vec2 h )
{
    vec3 q = abs(p);
    return max(q.z-h.y,max((q.x*0.866025+q.y*0.5),q.y)-h.x);
}

float sdCylinder( vec3 p, float r )
{
  return length(p.xz)-r;
}

float udBox( vec3 p, vec3 b )
{
  return length(max(abs(p)-b,0.0));
}

float sdSphere( vec3 p, float r)
{
  return length(p)-r;
}

mat3 rot(float x, float y, float z)
{
	float cx = cos(x);
	float sx = sin(x);
	float cy = cos(y);
	float sy = sin(y);
	float cz = cos(z);
	float sz = sin(z);
	mat3 xm = mat3(1, 0, 0,
					0, cx, -sx,
					0, sx, cx);
	mat3 ym = mat3(cy, 0, sy,
			  		0, 1, 0,
			  		-sy, 0, cy);
	mat3 zm = mat3(cz, -sz, 0,
					sz, cz, 0,
					0, 0, 1);
	return xm * ym * zm;
}

float specular(vec3 normal, vec3 light, vec3 viewdir, float s)
{
	float nrm = (s + 8.0) / (3.1415 * 8.0);
	float k = max(0.0, dot(viewdir, reflect(light, normal)));
    return  pow(k, s);
}



float smin( float a, float b)
{
	float k = 0.5;
    float h = clamp( 0.5+0.5*(b-a)/k, 0.0, 1.0 );
    return mix( b, a, h ) - k*h*(1.0-h);
}

float smink( float a, float b, float k )
{
    float h = clamp( 0.5+0.5*(b-a)/k, 0.0, 1.0 );
    return mix( b, a, h ) - k*h*(1.0-h);
}
vec2 sun(vec2 a, vec2 b)
{
	float sm = smin(a.x,b.x);
	float ca = abs(sm -a.x);
	float cb = abs(sm -b.x);

	return ca < cb ? vec2(sm, a.y) : vec2(sm, b.y);
}

vec2 sunk(vec2 a, vec2 b, float k)
{
	float sm = smink(a.x,b.x, k);
	float m = min(a.x, b.x);
	float ca = abs(sm -a.x);
	float cb = abs(sm -b.x);
	return ca < cb ? vec2(sm, a.y) : vec2(m, b.y);
}


#define TEX 0.2
#define SIZE 8.0
#define PI 3.1415

#define MAT_WALL 1.0
#define MAT_SPIN 2.0
#define MAT_GROUND 3.0


vec2 ground(vec3 p) {
	p -= vec3(0,0,5);
	p.y += 10*iGlobalTime;
	float s = 1.0;
	vec3 q = mod(p, s) - s * 0.5;
	q.z = p.z;

	q.z += sin(p.x) * 0.5;
	q.z += sin(p.y) * 0.5;
	float d = udRoundBox(q, vec3(s * 0.3), s * 0.3);
	return vec2(d, MAT_GROUND);
}

vec2 spin(vec3 p, vec3 rd, float dir) {
//	p.z += sin(p.x) * 0.5;
//	p.z += sin(p.y) * 0.5;
	p.x = p.x == 0 ? 0.00001 : p.x;

	float len = length(p.xy);
	float lenSize = 6.0;
	float lenY = mod(len, lenSize) - lenSize * 0.5;
	float lenPart = floor(len / lenSize);
	if (lenPart < 0.5 || lenPart > 2.5) {
		return vec2(99999,  MAT_SPIN);
	}
	float angle = atan(p.y, p.x);
	float numParts =  floor( 5.5*lenPart);
	float partSize = PI / numParts;
	angle += PI;
	float r = dir * sign(cos(lenPart*PI))* iGlobalTime * 0.4 / lenPart;
	float part = floor(mod(angle + r + (lenPart < 1.5 ? partSize / 2.0 : 0), PI * 2) / partSize);
//	angle += lenPart;
	angle = mod(0.3*lenPart + angle + r, partSize) + partSize * 0.5 * max(0.0, numParts - 1.0);

//	float lenPart = floor(len / lenSize);
	vec3 newPos = vec3(len * cos(angle), lenY * sin(angle), p.z /*+ sin(lenPart*iGlobalTime)*/);
	float d = sdSphere(newPos, 1.0/* +  0.0 * midi[00 + int(part)] *//* + 0.4*(sin(1.8*lenPart) +2)*/ );
//	float d = sdCylinder(newPos.xzy, 1.5 );
	return vec2(d, MAT_SPIN);
}



vec2 map(vec3 p, vec3 rd) {
//	vec2 wall = vec2(-sdBox(p, vec3(25)), MAT_WALL);
	vec2 res = spin(p ,rd, 1);
//	res = un(spin(p,rd, 1), res);
//	res = sun(res, spin	(p, rd, -1));
	res = un(ground(p - vec3(0, 0, 0)), res);
 	return res;
}

vec4 roofLight(vec3 p) {
	float music = 1.0; //midi[46];
	float s = 15.0;
	p.y += 10*iGlobalTime;
	vec3 q = mod(p, s) - s * 0.5;
	q.z = p.z;
	q *= rot(0,0,iGlobalTime * 5 + p.x*0.1 + p.z*0.1);
	vec3 lightCol = vec3(0.5 + 0.5 * sin(p.x), 0.5 + 0.5 * sin(p.y), 0.1);
	float dis = sdCappedCylinder(q.zxy - vec3(-10, 0, 0), vec2(0.05, 5.0));
//	float dis = sdTorus88(q.zxy - vec3(-10, 0, 0), vec2(0.5, 0.01));
	float distanceToL = max(0.0001, dis);
	vec3 point = lightCol * 5.0/(0.1*distanceToL + 0.3*distanceToL*distanceToL);

	return vec4(point, distanceToL);
}

vec4 sun(vec3 pos) {
	vec3 lightCol = vec3(1,0.8, 0.5*(cos(iGlobalTime) + 1));

	float music = 1.0;// midi[46];//61
	float mdis = sdSphere(pos - vec3(0,0,0.5*sin(10*iGlobalTime)), 0.8 + 0.8 * music);


	float distanceToL = max(0.0001, mdis);
	vec3 point = lightCol * (80.0 + 80 * music)/(distanceToL*distanceToL);

	return vec4(point, distanceToL);
}


vec3 evaluateLight(vec3 pos, inout float dis)
{
	vec4 sun = sun(pos);
	vec4 rl = roofLight(pos);
//	vec4 rl2 = roofLight(pos - vec3(0,0,20));
	dis = min(sun.w, rl.w);
//	dis = min(dis, rl2.w);
	return sun.xyz + rl.xyz/* + rl2.xyz*/;
}

void addLightning(inout vec3 color, vec3 normal, vec3 eye, vec3 pos) {

//	vec3 lpos = LPOS;//vec3(1, 1, -2);
	vec3 lpos = vec3(0, 0,0);

	float dis = length(lpos - pos);
	vec3 invLight = normalize(lpos - pos);
	float diffuse = max(0.0, dot(invLight, normal));
	float spec = specular(normal, -invLight, normalize(eye - pos), 220.0);

	float str = 1.0/(0.1 + 0.01*dis + 0.1*dis*dis);
	float tmp = 0;
	str = 1;
	color =  color * (0.0 + 0.8*diffuse*evaluateLight(pos, tmp).xyz) + spec*str;
//	color = clamp(color, vec3(0), vec3(1));
	//color *= str;
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



vec3 raymarch(vec3 ro, vec3 rd, inout vec3 finalPos, vec3 eye) {
	float t = 0.0;
	const int maxIter = 100;
	const float maxDis = 300.0;
	float d = 0.0;
	vec3 p = vec3(-1.0, -1.0, -1.0);
	vec3 col = vec3(0);
	const int jumps = 3;
	float ref = 1.0;
	vec3 scatteredLight = vec3(0.0);
	float transmittance = 1.0;
	for (int j = 0; j < jumps; j++) {
		for (int i = 0; i < maxIter; i++) {
			p = ro + rd * t;

			vec2 res = map(p, rd);
			d = res.x;
//			d = min(d, 0.2);
			float fogAmount = 0.01;
			float lightDis = -1.0;
			vec3 light = evaluateLight(p, lightDis);
			d = min(min(d, 1), max(lightDis, 0.05));
			vec3 lightIntegrated = light - light * exp(-fogAmount * d);
			scatteredLight += transmittance * lightIntegrated;
			transmittance *= exp(-fogAmount * d);

			t += d;
			float m = res.y;
			bool end = i == maxIter - 1 ||t > maxDis;
			if (d < 0.01 || end) {
				vec3 c = vec3(1);
				vec3 normal = getNormal(p, rd);
				if (m == MAT_WALL) {
					c = vec3(1,0,0);
					//c = vec3(0);
				} else if (m == MAT_SPIN) {
					c = vec3(0.5);
//					c = mix(c, texCube(iChannel0, p * 0.1, getNormal(p, rd), 4).xyz, 0.5);
				} else if (m == MAT_GROUND) {
					vec3 q = floor(p);
					//c = vec3((1 + sin(iGlobalTime + 0.3*q.x + 0.2*q.y))*0.5, (1 + cos(iGlobalTime))*0.5, (1 + cos(iGlobalTime + 1 ))*0.5);
					c = vec3(0.3,0.3,1);
					//c*=2;
					//c = mix(c, texCube(iChannel1, p * 0.01 + vec3(0, 10*iGlobalTime, 0) * 0.01, normal, 1.0).xyz, 0.81);
				}

				c *= occlusion(p, normal, rd);
				addLightning(c, normal, eye, p);
				if (end) {
					transmittance = 0;
				}
				col = mix(col, transmittance * c + scatteredLight, ref);
//				col = c;
//				col = mix(col, vec3(0.7), clamp(t * 0.01, 0, 1));
				if (m == MAT_SPIN) {
					ref *= 0.8;
				} else {
					ref = 0.0;
				}
				rd = reflect(rd, getNormal(p, rd));
				ro = p + rd*0.05;
				t = 0;
				break;
			}
			if (t > maxDis) {
				break;
			}
		}

		if (ref < 0.1) {
			break;
		}
	}
	finalPos = p;
	return col;
}





void mainImage(out vec4 fragColor, in vec2 fragCoord)
{
	float u = (fragCoord.x / 1.0) * 2.0 - 1.0;
    float v = ((fragCoord.y / 1.0) * 2.0 - 1.0) * (9.0/ 16.0);//* (iResolution.y/iResolution.x);

    float t = iGlobalTime;
    vec3 start = vec3(0.3*sin(t), -24 + 2*cos(t) , -4 );

    float alpha = smoothstep(200.0, 201.0, iGlobalTime);
	vec3 tar = vec3(0); //eye + vec3(0.0, 0.0, 1.0);
    vec3 eye = start + alpha*(tar - start);
    vec3 lol = vec3(0, 1, 0);

      if (t > 16) {
    	eye = vec3(0, 0, -(iGlobalTime + 4)*3 + 55);
    	tar = vec3(0.1);
    	lol = vec3(0, 0, -1);

        float alpha = smoothstep(25.0, 26.0, iGlobalTime);
        eye = eye + alpha*(tar - eye);
    } else if(t > 8) {
    	eye = vec3(11*cos(0.4*t + 1),11*sin(0.4*t+1),-0);
    	tar = vec3(0.1,1,0);
    	lol = vec3(0, 0, -1);
    }


	vec3 dir = normalize(tar - eye);
	vec3 right = normalize(cross(lol, dir));
	vec3 up = cross(dir, right);
	vec3 ro = eye;
	vec3 rd = normalize(dir + right*u + up*v);

	vec3 light = vec3(0.0, 0.0, 26.0 );

	vec3 finalPos = vec3(-1.0, -1.0, -1.0);
	float material = -1.0;
	vec3 color = raymarch(ro, rd, finalPos, eye);

    fragColor = vec4(color, 1.0);
    fragColor.rgb = fragColor.rgb / (fragColor.rgb + vec3(1.0));
}

/////////////////////////////////////////////////////
void main()
{
	mainImage(fragColor, fragCoord);
}

)"