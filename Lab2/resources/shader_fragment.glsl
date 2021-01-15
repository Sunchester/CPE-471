#version 330 core
out vec3 color;
uniform float yWindow;
in vec3 vertex_to_fragment_color;
in vec3 vertex_to_pixel_position;
void main()
{
	

	color = vec3(0.0, 0.0, 1.0);
	float dist1 = length(vertex_to_pixel_position);
	color = vertex_to_fragment_color;
	// Enable to do fade	
	// color = vertex_to_fragment_color + vec3(dist1, dist1, dist1);
	// Enable to do halfway
//	if(gl_FragCoord.y >= yWindow)
//	{
//		color = vec3(0.0, 0.0, 1.0);
//	}
	float dist = distance(vertex_to_pixel_position, vec3(0.0,0.0,0.0));
	if(dist < 0.1)
	{
		discard;
	}

}
