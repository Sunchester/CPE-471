#version 330 core
out vec4 color;
in vec3 vertex_normal;
in vec3 vertex_pos;
in vec2 vertex_tex;
uniform vec3 campos;

uniform sampler2D tex;
uniform sampler2D tex2;
uniform sampler2D tex3;
uniform sampler2D texN;

void main()
{
	float factor = texture(tex3, vertex_tex).r;
	vec4 tcol = texture(tex, vertex_tex);
	tcol += texture(tex2, vertex_tex);
	vec4 tcolN = texture(texN, vertex_tex);
//	color = tcol;
//	// diffuse 
	vec3 n = normalize(vertex_normal);
	vec3 lp = vec3(40, 10, 10);
	vec3 ld = normalize(lp-vertex_pos);
	float d = dot(n,ld);
	d = clamp(d, 0, 1);
//	d = pow(d, 0.5);

	// specular
	vec3 cd = normalize(campos - vertex_pos);
	vec3 h = normalize(cd+ld);
	float s = dot(h,n);
	s = clamp(s, 0, 1);
	s= pow(s, 35);
	color = tcol * d + tcolN * (1-d);
//	color.rgb += vec3(1,1,1)*s;
	color.rgb += vec3(1,1,1)*s * factor;
}
