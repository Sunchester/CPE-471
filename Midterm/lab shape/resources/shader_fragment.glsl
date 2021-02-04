#version 330 core
out vec4 color;
in vec3 vertex_pos;
in vec3 vertex_color;
uniform vec3 black;
void main()
{
//color = vec3(1,1,1);
if(black.x == -1)
{
	color.rgb = vertex_color + black;
	color.a = 1;
}
else if(black.x == 0 && black.z == 0)
{
	color.rgb = vertex_color;
	color.a = 1;
}
else
{
	color.rgb = black;
	color.a = 1;
}
}
