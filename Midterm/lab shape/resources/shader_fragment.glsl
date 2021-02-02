#version 330 core
out vec3 color;
in vec3 vertex_pos;
in vec3 vertex_color;
void main()
{
//color = vec3(1,1,1);
color = vertex_color;
}
