#version 330 core
out vec3 color;
in vec3 vertex_color;
in vec3 vertex_position;
uniform vec3 diffcol;
void main()
{
	color = vertex_color;
	if(diffcol.x != 0)
	{
		color.rgb = diffcol.xyz;
	}
}
