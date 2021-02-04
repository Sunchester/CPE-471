#version 330 core
out vec4 color;
in vec3 vertex_pos;
in vec3 vertex_color;
in vec3 center;
uniform vec3 black;
void main()
{
//color = vec3(1,1,1);
color.rgb = black + center;
color.a = 0.15;

}
