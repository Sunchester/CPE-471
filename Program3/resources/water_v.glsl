#version 330 core
layout(location = 0) in vec3 vertPos;
layout(location = 2) in vec3 norm;
layout(location = 1) in vec2 vertTex;

uniform mat4 P;
uniform mat4 V;
uniform mat4 M;
out vec3 vertex_pos;
out vec2 vertex_tex;
out vec3 normal;
uniform sampler2D tex;
uniform vec3 camoff;

void main()
{
//	vec2 texcoords = vertTex;
////	float t = 1./100;
//	
	//texcoords/= 100.0f;
//	texcoords -= vec2(camoff.x,camoff.z)*t;
//	float baseheight = texture(tex,texcoords/100).r;
//	baseheight *= 150-45;
	
	vec4 tpos =  vec4(vertPos,1.0);
	normal = norm;
//	tpos.z-=camoff.z;
//	tpos.x-=camoff.x;
//
	tpos =  M * tpos;
	

//	float totalheight = 0;
//	float procedural1 = noise(tpos.xyz*10, 11, 0.03,0.6);
//	float procedural2 = noise(tpos.xyz*10, 4, 0.004, 0.3);
//	float procedural1 = noise(tpos.xyz*10, 50, 0.9,0.65);
//	float procedural2 = noise(tpos.xyz*10, 4, 0.001, 0.34);
//	procedural2 = pow(procedural2, 5)*3;
//	procedural1 = procedural1 * procedural2;
//	procedural1 *=60;
//
//	totalheight = 0.80 *baseheight + 0.20*procedural1;
//	tpos.y += totalheight;
	vertex_pos = tpos.xyz;
	gl_Position = P * V * tpos;
	vertex_tex = vertTex;


	//light

//	float g = 0.001;
//	float heightV = texture(tex, texcoords/100.).r;
//	float height1 = texture(tex, texcoords/100. + vec2(0,g)).r;
//	float height2 = texture(tex, texcoords/100. + vec2(g,0)).r;

//	vec3 a,b,c;
//	a = vec3(0,heightV*150-45,0);
//	b = vec3(1,height1*150-45,0);
//	c = vec3(0,height2*150-45,1);
//
//	vec3 n = -normalize(cross(a-b,a-c));
//
//	normal = n;
}
