#version 330 core
layout(location = 0) in vec3 vertPos;
layout(location = 1) in vec2 vertTex;

uniform mat4 P;
uniform mat4 V;
uniform mat4 M;
out vec3 vertex_pos;
out vec2 vertex_tex;
out vec3 normal;
uniform sampler2D tex;
uniform vec3 camoff;



float hash(float n) 
{ 
	return fract(sin(n) * 753.5453123); 
}
float snoise(vec3 x)
{
	vec3 p = floor(x);
	vec3 f = fract(x);f = f * f * (3.0 - 2.0 * f);
	float n = p.x + p.y * 157.0 + 113.0 * p.z;
	return mix(mix(mix(hash(n + 0.0), hash(n + 1.0), f.x),mix(hash(n + 157.0), hash(n + 158.0), f.x), f.y),mix(mix(hash(n + 113.0), hash(n + 114.0), f.x),mix(hash(n + 270.0), hash(n + 271.0), f.x), f.y), f.z);
}
float noise(vec3 position, int octaves, float frequency, float persistence)
{
	float total = 0.0;
	float maxAmplitude = 0.0;
	float amplitude = 1.0;
	for (int i = 0; i < octaves; i++) {
	total += snoise(position * frequency) * amplitude;frequency *= 2.0;maxAmplitude += amplitude;amplitude *= persistence;
	}
	return total / maxAmplitude;
}
void main()
{
	vec2 texcoords = vertTex;
	float t = 1./100;
	
	//texcoords/= 100.0f;
	texcoords -= vec2(camoff.x,camoff.z)*t;
	float baseheight = texture(tex,texcoords/100).r;
	baseheight *= 150-45;
	
	vec4 tpos =  vec4(vertPos,1.0);
	tpos.z-=camoff.z;
	tpos.x-=camoff.x;

	tpos =  M * tpos;
	

	float totalheight = 0;
	float procedural1 = noise(tpos.xyz*2, 11, 0.03,0.6);
//	float procedural2 = noise(tpos.xyz*10, 4, 0.004, 0.3);
//	float procedural1 = noise(tpos.xyz*10, 50, 0.9,0.65);
//	float procedural2 = noise(tpos.xyz*10, 4, 0.001, 0.34);
//	procedural2 = pow(procedural2, 5)*3;
//	procedural1 = procedural1 * procedural2;
	procedural1 *=40;

	totalheight = 0.80 *baseheight + 0.20*procedural1;
	tpos.y += totalheight;
	vertex_pos = tpos.xyz;
	gl_Position = P * V * tpos;
	vertex_tex = vertTex;


	//light

	float g = 0.001;
	float heightV = texture(tex, texcoords/100.).r;
	float height1 = texture(tex, texcoords/100. + vec2(0,g)).r;
	float height2 = texture(tex, texcoords/100. + vec2(g,0)).r;

	vec3 a,b,c;
	a = vec3(0,heightV*150-45,0);
	b = vec3(1,height1*150-45,0);
	c = vec3(0,height2*150-45,1);

	vec3 n = -normalize(cross(a-b,a-c));

	normal = n;
}
