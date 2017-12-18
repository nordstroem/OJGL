R""(
#version 430

in vec2 fragCoord;
out vec4 fragColor;

// 3D Scene
float udRoundBox( vec3 p, vec3 b, float r ) {
  return length(max(abs(p)-b,0.0))-r;
}

float map(vec3 p) {
    vec2 s = vec2(5.0);
    p.xz = mod(p.xz, s) - s * 0.5;
	//return min(udRoundBox(p, vec3(1.0), 0.1), p.y + 4.0);
    return min(length(p) - 1.5, min(p.y + 4.0, -p.y + 4.0));
}

vec3 getNormal(vec3 p) {
	vec3 normal;
    vec3 ep = vec3(0.01, 0, 0);
    normal.x = map(p + ep.xyz) - map(p - ep.xyz);
    normal.y = map(p + ep.yxz) - map(p - ep.yxz);
    normal.z = map(p + ep.yzx) - map(p - ep.yzx);
    return normalize(normal);

}

float specular(vec3 normal, vec3 light, vec3 viewdir, float s) {
	float k = max(0.0, dot(viewdir, reflect(light, normal)));
    return  pow(k, s);
}

void main() {
	float iGlobalTime = 0.0;
	vec2 uv = (fragCoord * 2.0) - 1.0;
    
    vec3 ro = vec3(5., 1. , 2.5); //vec3(sin(iTime * 0.25) * 5.0, 3.0, cos(iTime * 0.25) * 5.0);
    
    vec3 tar = vec3(0.0, 0.0, 2.5);
    vec3 dir = normalize(tar - ro);
	vec3 right = normalize(cross(vec3(0.0, 1.0, 0.0), dir));
	vec3 up = cross(dir, right);

	vec3 rd = normalize(dir + right*uv.x + up*uv.y);
    
    vec3 color = vec3(0.0);
    
    vec3 p = ro;
	vec3 firstP = ro;
    vec3 start = ro;
    vec3 end = vec3(0.0);
    float ref = 0.0;
    for (int jump = 0; jump < 5; jump++) {
        float t = 0.0;
        for (int i = 0; i < 100; i++) {
            p = ro + rd * t;
            float d = map(p);     
            if (d < 0.01) {
                if (jump == 0) {
                    end = p;
                }
               	ref = 0.5;
                vec3 col = vec3(0.);
                if (p.y < - 3.0) {
                    ref = 0.0;
                    if (mod(floor(p.x) + floor(p.z), 2.0) > 0.5 ) {
                    	 col = vec3(0.9, 0.2, 0.9);
                    } else {
                    	 col = vec3(0.5, 0.2, 0.9);
                    }
                   
                } else if ( p.y > 3.0) {
                    ref = 0.0;
                    if (mod(floor(p.x) + floor(p.z), 2.0) > 0.5 ) {
                    	 col = vec3(0.7, 0.5, 1.0);
                    } else {
                    	 col = vec3(0.3, 0.2, 1.0);
                    }
                    
                } else {
                    ref = 0.5;
                	col = vec3(1.0); 
                    
                }

                vec3 lpos = ro;
                float dis = length(lpos - p);
                vec3 invLight = normalize(lpos - p);
                vec3 normal = getNormal(p);
                float diffuse = max(0.0, dot(invLight, normal));
                float spec = specular(normal, -invLight, normalize(ro - p), 250.0);
                float str = 1.0;//1.0/(0.1*dis);
                col = col * 0.8 * diffuse*str + spec * str * 5.0;
              
				rd = reflect(rd, normal);
                ro = p + rd * .1;
                color = mix(col, color, ref);
				break;               
                
            }
            t += d;
        }
		if (jump == 0) {
			firstP = p;
		}
        if (ref < 0.01) {
        	break;
        }
    }



    float focus = abs(length(firstP - start) - (8.0 - 7.0 * sin(iGlobalTime * 1.0))) * 0.1;
    focus = min(focus, 1.);
	fragColor = vec4(color, focus);
}

)""