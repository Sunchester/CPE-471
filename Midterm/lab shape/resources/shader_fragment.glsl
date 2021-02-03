#version 330 core
out vec3 color;
in vec3 vertex_pos;
in vec3 vertex_color;
uniform vec3 black;
void main()
{
//color = vec3(1,1,1);
if(black.x == -1)
	color = vertex_color + black;
else if(black.x == 0)
	color = vertex_color;
else
	color = black;
}
