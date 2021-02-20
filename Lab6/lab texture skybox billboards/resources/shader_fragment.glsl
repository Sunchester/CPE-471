#version 330 core
out vec4 color;
in vec3 vertex_pos;
in vec2 vertex_tex;

uniform sampler2D tex;
uniform vec2 offset;
void main()
{
vec4 tcol = texture(tex, vertex_tex *0.25 + offset);
color = tcol;
color.a = (tcol.x + tcol.y + tcol.z)/3;
}
