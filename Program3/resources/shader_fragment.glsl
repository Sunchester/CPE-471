#version 330 core
out vec4 color;
in vec3 vertex_pos;
in vec2 vertex_tex;
in vec3 normal;
uniform vec3 campos;

uniform sampler2D tex;
uniform sampler2D tex2;
uniform vec3 camoff;

void main()
{
vec2 texcoords = vertex_tex;
float t = 1./100;
texcoords -= vec2(camoff.x,camoff.z)*t;

vec3 heightcolor=  texture(tex, texcoords/100).rgb;
heightcolor.r = 0.4 + heightcolor.r*0.6;
color.rgb = texture(tex2, texcoords*100).rgb * heightcolor.r;



// !!! HELLO OVER HERE ENABLE THIS TO SHOW THE RED LINE!!!
// AND DISABLE SOME THINGS DOWN BELOW!
//color.rgb = texture(tex2, texcoords/100).rgb;
////

color.a = 1;


float len = length(vertex_pos.xz + campos.xz);
len -= 45;
len/=4.;
len = clamp(len, 0, 1);
color.a = 1-len;


//Light Diffuse
vec3 lp = vec3(300,100,300);
vec3 ld = lp - vertex_pos;
float diff = dot(normalize(ld), normalize(normal));
diff = clamp(diff, 0, 1);



//Light Spec
float factor = texture(tex, texcoords/100).r;
vec3 cd = normalize(campos - vertex_pos);
vec3 h = normalize(cd+ld);
float s = dot(h,normal);
s = clamp(s, 0, 1);
s= pow(s, 35) * factor;


// DISABLE THESE!
color.rgb *= diff;
color.rgb += vec3(1,1,1)*s;
// YEP 
}
