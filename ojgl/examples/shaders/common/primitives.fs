R""(
float sdPlane(vec3 p, vec4 n)
{
    return dot(p, n.xyz) + n.w;
}
float sdCappedCylinder(vec3 p, vec2 h)
{
    vec2 d = abs(vec2(length(p.xz), p.y)) - h;
    return min(max(d.x, d.y), 0.0) + length(max(d, 0.0));
}
float sdSphere(vec3 p, float s)
{
    return length(p) - s;
}
float sdBox(vec3 p, vec3 b)
{
    vec3 d = abs(p) - b;
    return length(max(d, 0.0)) + min(max(d.x, max(d.y, d.z)), 0.0);
}
float sdRoundBox(vec3 p, vec3 b, float r)
{
    vec3 q = abs(p) - b;
    return length(max(q, 0.0)) + min(max(q.x, max(q.y, q.z)), 0.0) - r;
}
float sdRoundCone( vec3 p, float r1, float r2, float h )
{
  vec2 q = vec2( length(p.xz), p.y );
    
  float b = (r1-r2)/h;
  float a = sqrt(1.0-b*b);
  float k = dot(q,vec2(-b,a));
    
  if( k < 0.0 ) return length(q) - r1;
  if( k > a*h ) return length(q-vec2(0.0,h)) - r2;
        
  return dot(q, vec2(a,b) ) - r1;
}
float sdTorus( vec3 p, vec2 t )
{
  vec2 q = vec2(length(p.xz)-t.x,p.y);
  return length(q)-t.y;
}
float sdHalfTorus(vec3 p, vec2 t)
{
  vec2 q = vec2(length(p.xy)-t.x,p.z);
  float d = length(q)-t.y;
	if (p.y > 0) {
		d = max(d, p.y);
	}
	return d;
}
float sdCylinder( vec3 p, float r)
{
  return length(p.xy)-r;
}
float sdVerticalCapsule( vec3 p, float h, float r )
{
  p.y -= clamp( p.y, 0.0, h );
  return length( p ) - r;
}

)""
