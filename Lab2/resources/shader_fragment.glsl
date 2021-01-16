#version 330 core
out vec3 color;
uniform float yWindow;
uniform float time;
in vec3 vertex_to_fragment_color;
in vec3 vertex_to_pixel_position;
void main()
{
	

	color = vec3(0.0, 0.0, 1.0);
	float dist1 = length(vertex_to_pixel_position);
	color = vertex_to_fragment_color;	 
	// Enable to do halfway
//	if(gl_FragCoord.y >= yWindow)
//	{
//		color = vec3(0.0, 0.0, 1.0);
//	}
	float factor = 8;
    float timeCalc = time * factor;
	float xTime = cos(timeCalc)*0.5;
	float yTime = sin(timeCalc)*0.5;
	float dist = distance(vertex_to_pixel_position, vec3(xTime,yTime,0));
	color += vec3(dist, dist, dist);
	if(dist < 0.1)
	{
		discard;
	}

}
