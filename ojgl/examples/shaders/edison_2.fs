R""(
#version 430

in vec2 fragCoord;
out vec4 fragColor;

uniform float iTime;
uniform vec2 iResolution;
uniform float DEBUG_D1;
uniform float DEBUG_D2;
uniform float DEBUG_D3;

// Globals
vec3 lightPosition = vec3(4.0, 0, 4);

/// start of boiler-plate

#define PI 3.14159265

vec3 palette( in float t, in vec3 a, in vec3 b, in vec3 c, in vec3 d )
{
    return a + b*cos( 6.28318*(c*t+d) );
}

mat2 rot(float a)
{
    return mat2(cos(a),sin(a),-sin(a),cos(a));
}

float pMod1(inout float p, float size) {
	float halfsize = size*0.5;
	float c = floor((p + halfsize)/size);
	p = mod(p + halfsize, size) - halfsize;
	return c;
}

void moda (inout vec2 p, float rep)
{
    float per = 2.*PI/rep;
    float a = atan(p.y, p.x);
    float l = length(p);
    a = mod(a-per*0.5,per)-per*0.5;
    p = vec2(cos(a),sin(a))*l;  
}

void mo (inout vec2 p, vec2 d)
{
    p = abs(p)-d;
    if(p.y >p.x) p = p.yx;
}

float stmin (float a, float b, float k, float n)
{
    float st = k/n;
    float u = b-k;
    return min(min(a,b), 0.5*(u+a+abs(mod(u-a+st,2.*st)-st)));
}


float smoothspike(float left, float right, float value) 
{
	float mid = (left + right) / 2.;
    return value < mid ? smoothstep(left, mid, value) : 1. - smoothstep(mid, right, value);
}


float hash11(float p) {
    return fract(sin(p * 727.1)*435.545);
}
float hash12(vec2 p) {
	float h = dot(p,vec2(127.1,311.7));	
    return fract(sin(h)*437.545);
}
vec3 hash31(float p) {
	vec3 h = vec3(127.231,491.7,718.423) * p;	
    return fract(sin(h)*435.543);
}

float noise_2( in vec2 p ) {
    vec2 i = floor( p );
    vec2 f = fract( p );	
	vec2 u = f*f*(3.0-2.0*f);
    return mix( mix( hash12( i + vec2(0.0,0.0) ), 
                     hash12( i + vec2(1.0,0.0) ), u.x),
                mix( hash12( i + vec2(0.0,1.0) ), 
                     hash12( i + vec2(1.0,1.0) ), u.x), u.y);
}
// 3d noise
float noise_3(in vec3 p) {
    vec3 i = floor(p);
    vec3 f = fract(p);	
	vec3 u = f*f*(3.0-2.0*f);
    
    vec2 ii = i.xy + i.z * vec2(5.0);
    float a = hash12( ii + vec2(0.0,0.0) );
	float b = hash12( ii + vec2(1.0,0.0) );    
    float c = hash12( ii + vec2(0.0,1.0) );
	float d = hash12( ii + vec2(1.0,1.0) ); 
    float v1 = mix(mix(a,b,u.x), mix(c,d,u.x), u.y);
    
    ii += vec2(5.0);
    a = hash12( ii + vec2(0.0,0.0) );
	b = hash12( ii + vec2(1.0,0.0) );    
    c = hash12( ii + vec2(0.0,1.0) );
	d = hash12( ii + vec2(1.0,1.0) );
    float v2 = mix(mix(a,b,u.x), mix(c,d,u.x), u.y);
        
    return max(mix(v1,v2,u.z),0.0);
}

// fBm
float fbm3(vec3 p, float a, float f) {
    return noise_3(p);
}

float fbm3_high(vec3 p, float a, float f) {
    float ret = 0.0;    
    float amp = 1.0;
    float frq = 1.0;
    for(int i = 0; i < 4; i++) {
        float n = pow(noise_3(p * frq),2.0);
        ret += n * amp;
        frq *= f;
        amp *= a * (pow(n,0.2));
    }
    return ret;
}


mat3 rotateAngle(vec3 v, float a )
{
    float si = sin( a );
    float co = cos( a );
    float ic = 1.0f - co;

    return mat3( v.x*v.x*ic + co,       v.y*v.x*ic - si*v.z,    v.z*v.x*ic + si*v.y,
                   v.x*v.y*ic + si*v.z,   v.y*v.y*ic + co,        v.z*v.y*ic - si*v.x,
                   v.x*v.z*ic - si*v.y,   v.y*v.z*ic + si*v.x,    v.z*v.z*ic + co );
}



const float EPS = 2e-2;
const int MAX_STEPS = 100;

const float T_INF = -1.0;
const float T_SPHERE = 0.0;
const float T_WALL = 1.0;
const float T_BOX = 2.0;
const float T_ARROW = 3.0;

const vec3[] arrs = vec3[20](
vec3(0.46236206, 0.70231345 , 0.53169691), 
vec3(0.2132808  , 0.22293179 , 0.94648516),
vec3(-0.05071868 ,  0.6621354   , 0.74163369),
vec3(-0.0552707  ,  0.52395075  , 0.84528939),
vec3(0.39465571 , 0.58627051 , 0.70596794),
vec3(0.29108856,  0.7714463  , 0.55812972),
vec3(0.37339145 , 0.67291647 , 0.63510697),
vec3(0.01751453 , 0.58879056 , 0.80503947),
vec3(0.02804069 , 0.73287578 , 0.67357057),
vec3(0.46907611 , 0.62152748 , 0.62259924),
vec3(0.34878045 , 0.27573    , 0.89228966),
vec3(-0.01338117  , 0.70767689  , 0.70012086),
vec3(0.23661175 , 0.64417871 , 0.72637579),
vec3(0.06150907 , 0.46257157 , 0.88213247),
vec3(0.21010989 , 0.29683322 , 0.92846117),
vec3(-0.05105933  , 0.59853327  , 0.79453398),
vec3(0.38651043 , 0.74848105 , 0.52959447),
vec3(-0.08002648  , 0.62211056  , 0.77266498),
vec3(0.15974723 , 0.27943837 , 0.94296055),
vec3(0.15827769 , 0.45903349 , 0.87321441)

);
    
float psin(float v) 
{
    return (1.0 + sin(v)) * 0.5;
}

float reflectiveIndex(float type) 
{
    if (type == T_WALL)
        return 0.4 + 0.*smoothspike(3., 5., mod(iTime, 5.0));
	if (type == T_SPHERE)
        return 0.2;
	if (type == T_BOX)
        return 0.5;
	return 0.0;
}

float specularFactor(float type) 
{
    if (type == T_WALL)
        return 0.0;
	return 1.0;
}



vec3 color(float type, vec3 p) 
{
    if (type == T_WALL)
		 //return vec3(0.2);
       return palette(fract(-p.x*p.y*0.015), vec3(0.5), vec3(0.5), vec3(1.0, 1.0, 1.0), vec3(0.00, 0.10, 0.20	) );
    else if (type == T_SPHERE)
        return vec3(0.2, 0.1, 0.6);
    else if (type == T_BOX) {
        
        vec3 white = vec3(0., 1.0, DEBUG_D1);
        vec3 red = vec3(1.0, 0, 0);
		float m = mod(floor((iTime+1.9) * 1. / 2.), 2.);
        red = mix(white, red, 1);
        
        vec3 yellow = vec3(1.0, 1.0, 0.0);
        //return palette(fract(p.x*p.y*0.02), vec3(0.5), vec3(0.5), vec3(1.0, 1.0, 0.5), vec3(0.80, psin(iTime), 0.30	) );

		//return palette(fract(-p.x*p.y*0.05), vec3(0.5), vec3(0.5), vec3(	2.0, 1.0, 0.0), vec3(0.50, 0.20, 0.25) );
        //return red;
		return mix(red, yellow, 0.3*noise_3(2.0*sin(iTime*0.5)*p + iTime*0.5));
     }
    else if (type == T_ARROW)
        return vec3(0.1, 0.1, 0.8);
    return vec3(0.0);
}

float sdPlane( vec3 p, vec4 n )
{
  // n must be normalized
  return dot(p,n.xyz) + n.w;
}


float sdCappedCylinder( vec3 p, vec2 h )
{
  vec2 d = abs(vec2(length(p.xz),p.y)) - h;
  return min(max(d.x,d.y),0.0) + length(max(d,0.0));
}


float sdSphere( vec3 p, float s )
{
  return length(p)-s;
}

float sdBox(vec3 p, vec3 b)
{
  vec3 d = abs(p) - b;
  return length(max(d,0.0)) + min(max(d.x,max(d.y,d.z)),0.0);
}


// O
float sdTorus(vec3 p, vec2 t)
{
  p.y *= 0.7;
  p.zy = p.yz;
  vec2 q = vec2(length(p.xy)-t.x,p.z);
  return length(q)-t.y;
}

float sdTorusJ(vec3 p, vec2 t)
{
    p.y -= 2.0;
  //p.zy = p.yz;
  vec2 q = vec2(length(p.xy)-t.x,p.z);
  float d = length(q)-t.y;

	if (p.y > 0.0) {
		d = max(d, p.y);
	}
    
   	 d = min(d, sdCappedCylinder(p, vec2(0, 1)));

	return d;
}


float smink( float a, float b, float k )
{
    float h = clamp( 0.5+0.5*(b-a)/k, 0.0, 1.0 );
    return mix( b, a, h ) - k*h*(1.0-h);
}

vec2 sun(vec2 a, vec2 b)
{
    float sm = smink(a.x,b.x, 0.1);
	float m = min(a.x, b.x);
	float ca = abs(sm -a.x);
	float cb = abs(sm -b.x);
	
	return ca < cb ? vec2(sm, a.y) : vec2(m, b.y);
}

vec2 un(vec2 a, vec2 b) { return a.x < b.x ? a : b; }

float rv(float low, float high, float p)
{
    return low + (high - low) * hash11(p);
}

float note() {
	return mod(iTime, 5.0);     
}

float arrows(vec3 p)
{
	float d = 13337.0;
    float t = mod(iTime, 5.0);
    
   /* float hbox = length(p - vec3(0.0, 1.0, -0.2)) - 1.0;
    vec3 a = normalize(vec3(0.0, 1.0, -0.2));
    float th0 = 3.14 / 8.0;
	vec3 u = cross(a, vec3(1, 0, 0));
    vec3 v = cross(a, u); // These vectors can be hardcoded.
	float d = 13337.0;
    float t = mod(iTime, 5.0);*/
    vec2 c = vec2(3.0);
	vec3 q = p;
    q.xz = mod(p.xz, c) - 0.5 * c;
    q.x = abs(q.x);
    for(int i = 0; i < 20; ++i)
    {
    	/*float z = rv(cos(th0), 1.0, float(i));
        float th = acos(z);
        float phi = rv(0.0, 2.0 * 3.1415, float(i) * 200.0);
		vec3 x = 1.0 * sin(th) * (cos(phi) * u + sin(phi) * v) + cos(th) * a;
        x = x * t + vec3(0.0, -0.3, 0.0) * t * t;
		*/
        vec3 x = arrs[i];
        x = x * t + vec3(0.0, -0.3, 0.0) * t * t;
        d = min(d, length(q - x) - 0.05);
    }
    return d;
}

/*vec2 map(vec3 p)
{
    vec2 sphere = vec2(length(p - vec3(7.5 - 3.5*psin(iTime) + noise_3(p), 0.0, 0.0)) - 0.5, T_SPHERE);
    
    float ns = 0.2*noise_3(4.0 *( p - vec3(0.5*iTime, 0.1 * iTime, 0.2 * iTime)));
    vec2 box = vec2(sdBox(    p - vec3(3.0, 0.0, 0.0), vec3(1.5, 0.5, 0.5) + ns), T_BOX);
    vec2 walls = vec2(-sdBox( p - vec3(0.0, 0.0, 0.0), vec3(8.0)), T_WALL);
	vec2 arr = vec2(arrows(p), T_ARROW);
    return un(arr, un(box, un(sphere, walls)));
}*/


vec2 fractalBox(in vec3 p)
{
   p.xz *= rot(iTime*0.5);
   //p.yz *= rot(iTime*0.5);
   moda(p.xy, 15.);
   //p.x-=psin(iTime)*2.3;
   //moda(p.zy,  1.+ 5.0*psin(iTime));
   //p.z -= 2.0 * psin(iTime); 

    //   moda(p.zy,  1+DEBUG_D1 / 5);
   //p.z -= DEBUG_D2 / 5; 
   pMod1(p.x, 1.0);
   
  // mo(p.xz, vec2(DEBUG_D1/10, DEBUG_D2/10));
   //p.xy *= rot(sin(iTime)*p.y);
    float d = sdBox(p,vec3(1.0)) - 0.0;
   //d = mix(sdSphere(p, 1.0),d, 0.5 + 0.6*psin(iTime));
    
   vec2 res = vec2( d, T_BOX);

   float tim = mod(iTime + 1.7 - 2.0, 4.0);
   float s = 0.7 + 0.3*smoothstep(1.7, 2.0, tim) - 0.3*smoothstep(3.7, 4.0, tim);
   //float s = 0.7 + 0.2*smoothspike(0.1, 0.5, mod(iTime, 1.0));

   for( int m=0; m<3; m++ )
   {
      vec3 newp = p;
      vec3 a = mod( newp * s, 2.0 ) - 1.0;
      s *= 3.0;
      vec3 r = abs(1.0 - 3.0*abs(a));
      float da = max(r.x,r.y);
      float db = max(r.y,r.z);
      float dc = max(r.z,r.x);
      float c = (min(da,min(db,dc))-1.0)/s;

      if( c>d )
      {
          d = c;
          res = vec2( d, T_BOX);
       }
   }
    

   return res;
}

vec2 walls(vec3 p) {
    
 	float d = -sdBox( p - vec3(0.0, 10.0, 0.0), vec3(60.0));
    return vec2(d, T_WALL);
}

vec3 ballPos() {
 	return vec3(0.0, 1., 0.0);    
}

vec2 flooring(vec3 p) {
  return vec2(p.y +1.7 + 0.02*noise_2(2.*p.xz + 0.5*iTime), T_WALL);
}

vec2 map( in vec3 p )
{
    vec2 s = vec2(sdSphere(p - ballPos(), 1.0), T_BOX);
    //vec2 w = vec2(sdPlane(p, vec4(0.0, 1.0, 0.0, 1.0)) + 0.1*noise_3(p + vec3(20, 0.0, 20)), T_WALL);
    
    
    // + 0.01*noise_2(10.0*p.xz + vec2(iTime, iTime*1.5)) Ripple effect.
    vec2 w = flooring(p);
    
    float sc = 2.0;
    float tf = 0.5;
    vec3 direction = normalize(vec3(sin(iTime*tf), cos(iTime*tf), psin(iTime*tf)));
    vec2 fs = fractalBox(   1./sc*(p - ballPos()));
    fs.x *= sc;
    //float oj = sdTorus(p - vec3(sin(iTime), 0.18, 0.0), vec2(1.0, 0.02));
   // oj = min(99999999999.9, sdTorusJ(p, vec2(1.0, 0.02)));
   // vec2 fs = vec2(oj, T_BOX);
    
    //fs.x = mix(sdBox(p - ballPos(), vec3(1.0)), fs.x, 0.2+0.8*psin(0.5*iTime));
    return sun(w, fs);
}

vec3 normal(vec3 p) 
{
    float eps = EPS;
    vec3 n = vec3(map(vec3(p.x + eps, p.y, p.z)).x, map(vec3(p.x, p.y + eps, p.z)).x, map(vec3(p.x, p.y, p.z + eps)).x);
    return normalize(n - map(p).x);
}

vec2 march(vec3 ro, vec3 rd, out vec3 p, out int steps)
{
    float t = 0.0;
   	vec2 res = vec2(0.0, -1.0);
    for(steps = 0; steps < MAX_STEPS; ++steps) {
    	p = ro + t * rd;   
        vec2 tres = map(p);
        t += tres.x;
        if (tres.x < EPS) {
			res = tres;
            break;
        }
        if (t > 400.0) {
            break;
        }
    }
    return res;
}

float shadow(vec3 ro, vec3 dir) 
{
    float t = 0.01;
 	float sf = 1.0;
    for(int i = 0; i < MAX_STEPS; ++i) {
		vec3 p = ro + t * dir;    	
        vec2 res = map(p);
        t += clamp(res.x, 0.02, 0.1);
        if (res.x < 0.001)
            return 0.5;
       sf = min(sf, 8.0 * abs(res.x) / t);
    }
 	return min(1.0, 0.5 + 0.5*sf);
}

float ambientOcclusion(vec3 p, vec3 n) 
{
	float as = 0.0;
    float sl = 60.0 * 1e-3;
    int ns = 6;
    for(int i = 0; i < ns; i++) {
    	vec3 ap = p + float(i) * sl * n;    
    	as += map(ap).x;
    }
    return mix(1.0, smoothstep(0.0, float(ns *(ns - 1) / 2) * sl, as), 0.6);
}

//iq
float calcAO( in vec3 pos, vec3 nor)
{
	float occ = 0.0;
    float sca = 1.0;
    for( int i=0; i<5; i++ )
    {
        float hr = 0.01 + 0.12*float(i)/4.0;
        vec3 aopos =  nor * hr + pos;
        float dd = map( pos).x;
        occ += -(dd-hr)*sca;
        sca *= 0.95;
    }
    return clamp( 1.0 - 3.0*occ, 0.0, 1.0 );    
}

                          
vec3 colorize(vec2 res, vec3 p, vec3 dir, int steps) 
{
    vec3 light = normalize(vec3(1.0, -1,  -1.));
    vec3 lightPos = ballPos();
    //light = normalize(p - lightPos);
    
    vec3 n = normal(p);
    float lf = min(2.5, 3.0 / (0.02 + 0.1*pow(length(p - lightPos), 3)));
    
    // Material properties
    float diffuse1 = 0.0*max(0.,dot(-light, n));
    float diffuse2 = max(0.,dot(-normalize(p - lightPos), n));
    float diffuse = max(diffuse1, 2.*diffuse2);
    float k = max(0.0, dot(dir, reflect(-normalize(p - lightPos), n)));
    float spec = specularFactor(res.y) * pow(k, 100.0);
    
    vec3 col = color(res.y, p);
	float ao = ambientOcclusion(p, n);
   //	float sh = shadow(p, light);
    if (res.x < EPS)
        col =  (lf) * (ao * col *(0.02+diffuse) + spec);
    
    float ns = float(steps) / 100.;
    return pow(col, vec3(0.4545));
}

void main()
{
    vec2 uv = fragCoord - 0.5;
	uv.y *= iResolution.y / iResolution.x;
    vec3 ro = vec3(11.0, 10.+1.0*psin(iTime*0.)  , 11.0);
    vec3 tar = vec3(0.0, 1.0, 0.0);
    ro = mix(tar, ro, 1.0);
    vec3 dir = normalize(tar - ro);
	vec3 right = normalize(cross(vec3(0.0, 1.0, 0.0), dir));
	vec3 up = cross(dir, right);
	vec3 rd = normalize(dir + right * uv.x + up * uv.y);
    
    vec3 p;
    int steps;
    vec2 res = march(ro, rd, p, steps);
    vec3 col = colorize(res, p, rd, steps);

    /*vec3 lightPos = vec3(0.);
    float lDis = length(p - lightPos);
    float acc = 0.;
    for(float s = 0.; s < lDis; s+=0.1) {
		vec3 p2;
        vec3 ro2 = ro + rd * s;
        res = march(ro2, normalize(lightPos - ro2), p2, steps);
        float dis = length(ro2 - lightPos);
        if (length(p2 - ro2) > length(lightPos - ro2) || res.y == T_INF) {
           col += 0.3*1. / (dis*dis);
        }
    }*/
    float ri = reflectiveIndex(res.y);
    if (ri > 0.0) { 
        vec3 p2;
   		rd = reflect(rd, normal(p));
    	res = march(p + 0.1 * rd, rd, p2, steps);
    	vec3 newCol = colorize(res, p2, rd, steps);
    	col = mix(col, newCol, ri);
    }

    fragColor = vec4(col, 1.0);
}

)""  