vec4 texCube( sampler2D sam, in vec3 p, in vec3 n, in float k )
{
	vec4 x = texture2D( sam, p.yz );
	vec4 y = texture2D( sam, p.zx );
	vec4 z = texture2D( sam, p.xy );
    vec3 w = pow( abs(n), vec3(k) );
	return (x*w.x + y*w.y + z*w.z) / (w.x+w.y+w.z);
}