R""(
#version 430

in vec2 fragCoord;
layout(location = 0) out vec4 fragPos;
layout(location = 1) out vec4 fragNormal;

uniform float iTime;
uniform vec2 iResolution;

#define PI 3.1415

void moda (inout vec2 p, float rep)
{
    float per = 2.*PI/rep;
    float a = atan(p.y, p.x);
    float l = length(p);
    a = mod(a-per*0.5,per)-per*0.5;
    p = vec2(cos(a),sin(a))*l;  
}

float udRoundBox( vec3 p, vec3 b) {
  float r = 0.5;
  return length(max(abs(p)-b,0.0))-r;
}

float sdBox( vec3 p, vec3 b )
{
  vec3 q = abs(p) - b;
  return length(max(q,0.0)) + min(max(q.x,max(q.y,q.z)),0.0);
}

mat2 rot(float a)
{
    return mat2(cos(a),sin(a),-sin(a),cos(a));
}

float fractalBox(vec3 p) {
  float d = sdBox(p, vec3(1.0));

  moda(p.xz, 10.0);
  moda(p.xy, 10.0);
  p.yz = rot(iTime)*p.yz;

   float s = 1.0;
   for( int m=0; m<10; m++ )
   {
      vec3 a = mod( p*s, 2.0 )-1.0;
      s *= 3.0;
      vec3 r = abs(1.0 - 3.0*abs(a));

      float da = max(r.x,r.y);
      float db = max(r.y,r.z);
      float dc = max(r.z,r.x);
      float c = (min(da,min(db,dc))-1.0)/s;

      d = max(d,c);
   }
   return d;
}

void main(){
	const vec2 uv = fragCoord.xy;
    const vec3 ro = vec3(0.0, 0.0, 2.5);
    const vec3 rd = normalize(vec3(1.0, uv.y - 0.5, uv.x - 0.5));
    float t = 0.0;
    vec3 color = vec3(0.0);
    
    for (int i = 0; i < 10000000; i++) {
    	vec3 p = ro + rd * t;
        vec3 q = mod(p, 5.0) - 2.5;
        vec3 r = p / 8.0;
        float d = fractalBox(q);
        
        if (d < 0.01) {
            color = vec3(0.5 + 0.5 * sin(r * 1.3));
            
            const vec3 lpos = ro + vec3(-1.0, 0, 0);
            const float dis = length(lpos - p);
            const vec3 invLight = normalize(lpos - p);
            
            vec3 normal;
            const vec3 ep = vec3(0.01, 0, 0);
            normal.x = fractalBox(q + ep.xyz) - fractalBox(q - ep.xyz);
            normal.y = fractalBox(q + ep.yxz) - fractalBox(q - ep.yxz);
            normal.z = fractalBox(q + ep.yzx) - fractalBox(q - ep.yzx);
            normal = normalize(normal);
            
			fragNormal.rgb = normal;
			fragPos.rgb = p;
            break;
        }
        t += d;
    }    

	fragPos.a = 1.0;
	fragNormal.a = 1.0;
}

)""