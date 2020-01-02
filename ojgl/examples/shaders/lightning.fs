R""(
#version 430

in vec2 fragCoord;
out vec4 fragColor;

uniform sampler2D inTexture0;
uniform sampler2D inTexture1;
uniform float iTime;


float udRoundBox( vec3 p) {
  float r = 0.0;
  vec3 b = vec3(0.3);
  return length(max(abs(p)-b,0.0))-r;
}

float psin(float x) {
	return (1.0 + sin(x)) * 0.5;
}
void main()
{
	vec2 uv = fragCoord.xy;

	vec3 pos =  texture(inTexture0, uv).rgb;
	vec3 normal =  texture(inTexture1, uv).rgb;

	// Additional geometry
    vec3 ro = vec3(iTime, 0.0, 2.5);
    vec3 rd = normalize(vec3(1.0, uv.y - 0.5, uv.x - 0.5));
    float t = 0.0;

    for (int i = 0; i < 100; i++) {
    	const vec3 p = ro + rd * t;
        const vec3 q = mod(p, 5.0) - 2.5;
        const vec3 r = p / 8.0;
        const  float d = udRoundBox(q);
        
		// The "length(pos) > t" check determines that we hit this geometry instead of the precalculated
        if (d < 0.01 && length(pos) > t) {

            
            vec3 lpos = ro + vec3(-1.0, 0, 0);
            float dis = length(lpos - p);
            vec3 invLight = normalize(lpos - p);
            
            vec3 n;
            const  vec3 ep = vec3(0.01, 0, 0);
            n.x = udRoundBox(q + ep.xyz) - udRoundBox(q - ep.xyz);
            n.y = udRoundBox(q + ep.yxz) - udRoundBox(q - ep.yxz);
            n.z = udRoundBox(q + ep.yzx) - udRoundBox(q - ep.yzx);
            n= normalize(n);
            
			normal = n;
			pos = p;
            break;
        }
        t += d;
    }


	// Lightning
	vec3 lpos = vec3(50.0, 0.0, 0.0);
	{
		const float r = 30;
		const float speed = 1.0;
		lpos += vec3(cos(iTime * speed) * r, 0.1, sin(iTime * speed) * r);
	}

    const float dis = length(lpos - pos);
    const vec3 invLight = normalize(lpos - pos);

	const float diffuse = max(0.0, dot(invLight, normal));
    const float s = 10.0;
    const float k = max(0.0, dot(rd, reflect(invLight, normal)));
    const float spec =  pow(k, s);
    const float str = 60.0/(0.1 + 0.1*dis + 0.1*dis*dis);
	vec3 color = vec3(0.5, psin(iTime),psin(iTime + 1.6));
    color = color * (0.0 + 1.0*diffuse*str) + vec3(spec*str);

	// Final color
	fragColor.rgb = color;
	fragColor.a = 1.0;
}
)""