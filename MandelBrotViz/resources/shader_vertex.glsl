#version 450 compatibility
layout(location = 0) in vec3 vertPos;
//uniform mat4 P;
//uniform mat4 V;
//uniform mat4 M;
void main()
{
//	vec4 tpos =  M * vec4(vertPos, 1.0);
//	gl_Position = P * V * tpos;
//	pos = vertPos.xyz;
	gl_Position = vec4(vertPos.xyz, 1.0);

}
