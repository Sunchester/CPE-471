#version 330 core
out vec4 color;
in vec3 vertex_pos;
in vec2 vertex_tex;
uniform vec3 campos;

uniform sampler2D tex;
uniform sampler2D tex2;
uniform vec3 camoff;

void main()
{
vec2 texcoords = vertex_tex;
float t = 1./100;
texcoords -= vec2(camoff.x,camoff.z)*t;

vec4 b = texture(tex, texcoords);
vec3 heightcolor=  texture(tex, texcoords).rgb;
vec3 watercolor = texture(tex, texcoords).rgb;
heightcolor.r = 0.1 + heightcolor.r*0.9;
watercolor.r = 0.8 + watercolor.r;
if(b.x <= 0.15 && b.y <= 0.15 && b.z <= 0.15)
{
	color.rgb = vec3(156.0/255.0,211.0/255.0,219.0/255.0) * watercolor.r;
}
else
	color.rgb = texture(tex2, texcoords*100).rgb * heightcolor.r;
color.a = 1;
//color.rgb = vec3(1,1,1);
}
