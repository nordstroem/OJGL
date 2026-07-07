R""(
#define S_VOLUMETRIC 0
#define S_REFLECTIONS 0
struct x{float e;int i;};struct v{int i;vec3 r;int c;float t;vec3 v;int o;};struct n{float e;vec3 n;};
#define PI 3.14159265
in vec2 fragCoord;out vec4 fragColor;uniform float iTime;uniform vec2 iResolution;uniform mat4 iCameraMatrix;float f(vec3 v){v=abs(v)-vec3(1);return length(max(v,0.))+min(max(v.x,max(v.y,v.z)),0.);}mat2 f(float v){return mat2(cos(v),sin(v),-sin(v),cos(v));}x m(vec3 v){v.z+=5.;v.xy*=f(iTime);v.yz*=f(iTime*.5);return x(f(v),1);}vec3 S(vec3 v){vec3 i=vec3(m(vec3(v.x+.01,v.yz)).e,m(vec3(v.x,v.y+.01,v.z)).e,m(vec3(v.xy,v.z+.01)).e);return normalize(i-m(v).e);}vec3 S(v v){if(v.i!=-1){vec3 m=S(v.r);return vec3(.2,.5,.9)*(.1+.9*max(0.,dot(normalize(vec3(3,4,-3)-v.r),m)));}return vec3(0);}vec3 S(vec3 i,vec3 f){float r=0.;vec3 s=vec3(0);float u=1.,z=1.;vec3 y=vec3(0);
#if S_REFLECTIONS
for(int l=0;l<1;l++){
#else

#endif
for(int l=0;l<100;++l){vec3 c=i+r*f;x d=m(c);float e=d.e;
#if S_VOLUMETRIC
n P=n(1e6,vec3(0));e=min(e,max(.02,P.e*.75));vec3 k=max(vec3(0),P.n-P.n*exp(0.*e));s+=u*k;u*=exp(0.*e);
#endif
r+=e;if(d.e<.002){vec3 m=S(v(d.i,c,l,u,s,0));
#if!S_REFLECTIONS
return m;
#else
y=mix(y,m,z);z*=0.;if(z<.005)return y;r=0.;f=reflect(f,S(c));i=c+.1*f;break;
#endif
}if(r>4e2||l==99){vec3 m=S(v(-1,c,l,u,s,0));return mix(y,m,z);}}
#if S_REFLECTIONS
}
#endif
return y;}void main(){vec3 v=(iCameraMatrix*vec4(fragCoord.x-.5,(fragCoord.y-.5)*iResolution.y/iResolution.x,-1,1)).xyz;v=S(v,normalize(v-(iCameraMatrix*vec4(0,0,0,1)).xyz));fragColor=vec4(pow(v,vec3(.4545)),1);}
)""
