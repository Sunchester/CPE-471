#version 410 core

out vec4 color;
in vec2 fragTex;

uniform sampler2D tex;
uniform int onGamma;

void main()
{
	vec3 texturecolor = texture(tex, fragTex).rgb;
	color.rgb = texturecolor;
	color.a=1;
	float gamma = 1.55;
	if(onGamma == 1)
		color.rgb = pow(color.rgb, vec3(1.0/gamma));

}
