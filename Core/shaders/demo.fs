R"(
#version 430

in vec2 fragCoord; 
out vec4 fragColor;

uniform float iGlobalTime;

float udRoundBox( vec3 p, vec3 b, float r )
{
	return length(max(abs(p) - b, 0.0)) - r;
}

void main() 
{
	vec2 uv = fragCoord.xy;
	vec3 ro = vec3(iGlobalTime * 10.0, 2.0 * sin(iGlobalTime * 0.5), 2.0 * sin(iGlobalTime));
	vec3 rd = normalize(vec3(1.0, uv.y - 0.5, uv.x - 0.5));
	float t = 0.0;
	vec3 color = vec3(0.0);
	float b = (0.5 + 0.5 * sin(iGlobalTime * 10.0)) * 0.2 + 0.3;

	for (int i = 0; i < 100; i++) {
		vec3 p = ro + rd * t;
		vec3 q = mod(p, 8.0) - 4.0;
		vec3 r = p / 8.0;
		float d = udRoundBox(q, vec3(0.5), 0.3);

		if (d < 0.01) {
			color = vec3(0.5 + 0.5 * sin(r * 1.3));

			vec3 lpos = ro + vec3(-1.0, 0, 0); 
			float dis = length(lpos - p);
			vec3 invLight = normalize(lpos - p);

			vec3 normal;
			vec3 ep = vec3(0.01, 0, 0);
			normal.x = udRoundBox(q + ep.xyz, vec3(0.5), 0.3) - udRoundBox(q - ep.xyz, vec3(0.5), 0.3);
			normal.y = udRoundBox(q + ep.yxz, vec3(0.5), 0.3) - udRoundBox(q - ep.yxz, vec3(0.5), 0.3);
			normal.z = udRoundBox(q + ep.yzx, vec3(0.5), 0.3) - udRoundBox(q - ep.yzx, vec3(0.5), 0.3);
			normal = normalize(normal);

			float diffuse = max(0.0, dot(invLight, normal));
			float s = 10.0;
			float k = max(0.0, dot(rd, reflect(invLight, normal))); 
			float spec = pow(k, s);
			float str = 80.0 / (0.1 + 0.1*dis + 0.1*dis*dis);
			color = color * (0.0 + 1.0*diffuse*str) + spec*str;
			break;
		}
		t += d;
	}
	fragColor = vec4(color, 1.0);
	fragColor.rgb = fragColor.rgb / (fragColor.rgb + vec3(1.0));
}
)"