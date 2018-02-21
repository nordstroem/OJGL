vec4 texCube( sampler2D sam, in vec3 p, in vec3 n, in float k )
{
	vec4 x = texture2D( sam, p.yz );
	vec4 y = texture2D( sam, p.zx );
	vec4 z = texture2D( sam, p.xy );
    vec3 w = pow( abs(n), vec3(k) );
	return (x*w.x + y*w.y + z*w.z) / (w.x+w.y+w.z);
}

// Smooth min
float smin( float a, float b)
{
	float k = 0.1;
    float h = clamp( 0.5+0.5*(b-a)/k, 0.0, 1.0 );
    return mix( b, a, h ) - k*h*(1.0-h);
}

// Smooth min. k determines smoothness
float smink( float a, float b, float k )
{
    float h = clamp( 0.5+0.5*(b-a)/k, 0.0, 1.0 );
    return mix( b, a, h ) - k*h*(1.0-h);
}

// Union with smin
vec2 sun(vec2 a, vec2 b)
{
	float sm = smin(a.x,b.x);
	float ca = abs(sm -a.x);
	float cb = abs(sm -b.x);

	return ca < cb ? vec2(sm, a.y) : vec2(sm, b.y);
}

// Union with smink
vec2 sunk(vec2 a, vec2 b, float k)
{
	float sm = smink(a.x,b.x, k);
	float m = min(a.x, b.x);
	float ca = abs(sm -a.x);
	float cb = abs(sm -b.x);
	return ca < cb ? vec2(sm, a.y) : vec2(m, b.y);
}

// Water distance function
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