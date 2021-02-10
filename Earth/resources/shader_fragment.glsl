#version 330 core
out vec4 color;
in vec3 vertex_pos;
in vec3 normal;
uniform vec3 campos;

void main()
{
	color = vec4(0,1,1,1);

//	color.rgb = vertex_pos;
//	color.a = 1;
	// diffuse
	
	vec3 n = normalize(normal);
	vec3 lp = vec3(10,10,10);
	vec3 ld = normalize(lp-vertex_pos);
	float d = dot(n,ld);
	d = clamp(d, 0, 1);
//	d = pow(d, 5);
//

	// specular 
	vec3 cd = normalize(campos - vertex_pos);
	vec3 h = normalize(cd+ld);
	float s = dot(h,n);
	s = clamp(s, 0, 1);
	s= pow(s, 50);
	color.rgb *= d;
	color.rgb += vec3(1,1,1)*s;
//color = vec3(1,1,1);
//color = vertex_pos;
}
