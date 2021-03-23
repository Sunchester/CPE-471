#version 450 compatibility

out vec4 color;
uniform float amp;
uniform float amp2;
uniform float amp3;
uniform float amp4;
uniform float center_x;
uniform float center_y;
uniform float zoom;
uniform vec4 color_ranges;
uniform int movement;
in vec4 gl_FragCoord;
out float gl_FragDepth;
#define MAX_ITERATIONS 600

int get_iterations()
{
    double real = ((double(gl_FragCoord.x) / double(1080.0) - double(0.5)) * double(zoom) + double(center_x)) * double(4.0);
    double imag = ((double(gl_FragCoord.y) / double(1080.0) - double(0.5)) * double(zoom) + double(center_y)) *  double(4.0);    
    int iterations = 0;
    double const_real = real;
    double const_imag = imag;
 
    while (iterations < MAX_ITERATIONS)
    {
        double tmp_real = real;
        real = (real * real - imag * imag) + const_real;
        if(movement == 1)
            imag = ((double(2.75) - double(amp4)) * tmp_real * imag) + const_imag;
        else if(movement == 0)
            imag = (double(2.0) * tmp_real * imag) + const_imag;
        double dist = real * real + imag * imag;
        

        if (dist > 6.0)
            break;
 
        ++iterations;
    }
    return iterations;
}
vec4 return_color()
{
   int iter = get_iterations();
    if (iter == MAX_ITERATIONS)
    {
        gl_FragDepth = 0.0f;
        return vec4(0.0f, 0.0f, 0.0f, 1.0f);
    }
 
    double iterations = double(iter) / MAX_ITERATIONS;
    gl_FragDepth = float(iterations);
    vec4 color_0 = vec4(0.0f, 0.0f, 0.0f, 1.0f);
    vec4 color_1 = vec4(0.0f, 0.2f, 0.5f, 1.0f);
    vec4 color_2 = vec4(1.0f, 0.8f, 0.0f, 1.0f);
    vec4 color_3 = vec4(1.0f, 0.0f, 0.4f, 1.0f);
    
    float f;
    if(amp < 1./5.)
    {
    	f = (amp/5) * 5;
    	color_1.rgb = vec3(f, 0, 0);
    }
    else if(amp < 2./5.)
    {
    	f = (amp - 1./5.) * 5;
    	color_1.rgb = vec3(1,f,0);
    }
    else if(amp < 3./5.)
    {
    	f = (amp - 2./5.) * 5;
    	color_1.rgb = vec3(0,f,0);
    }
    else if(amp < 4./5.)
    {
    	f = (amp - 3./5.) * 5;
    	color_1.rgb = vec3(1-f,f,0);
    }
    else if(amp < 5./5.)
    {
    	f = (amp - 4./5.) * 5;
    	color_1.rgb = vec3(f,1,1-f);
    }
    else if(amp >= 5./5.)
    {
    	f = (amp - 5./5.) * 5;
    	color_1.rgb = vec3(f,f,f);
    }
    






    if(amp2 < 1./5.)
    {
    	f = (amp2/5) * 5;
    	color_2.rgb = vec3(0, 0, f);
    }
    else if(amp2 < 2./5.)
    {
    	f = (amp2 - 1./5.) * 5;
    	color_2.rgb = vec3(f,1,0);
    }
    else if(amp2 < 3./5.)
    {
    	f = (amp2 - 2./5.) * 5;
    	color_2.rgb = vec3(f,0,f);
    }
    else if(amp2 < 4./5.)
    {
    	f = (amp2 - 3./5.) * 5;
    	color_2.rgb = vec3(1-f,f,f);
    }
    else if(amp2 < 5./5.)
    {
    	f = (amp2 - 4./5.) * 5;
    	color_2.rgb = vec3(f,1,1-f);
    }
    else if(amp2 >= 5./5.)
    {
    	f = (amp2 - 5./5.) * 5;
    	color_2.rgb = vec3(f,f,1-f);
    }
    
 

    if(amp3 < 1./5.)
    {
    	f = (amp3/5) * 5;
    	color_3.rgb = vec3(f, 0, 0);
    }
    else if(amp3 < 2./5.)
    {
    	f = (amp3 - 1./5.) * 5;
    	color_3.rgb = vec3(f,1,0);
    }
    else if(amp3 < 3./5.)
    {
    	f = (amp3 - 2./5.) * 5;
    	color_3.rgb = vec3(0,f,f);
    }
    else if(amp3 < 4./5.)
    {
    	f = (amp3 - 3./5.) * 5;
    	color_3.rgb = vec3(1-f,f-1,0);
    }
    else if(amp3 < 5./5.)
    {
    	f = (amp3 - 4./5.) * 5;
    	color_3.rgb = vec3(f-1,1,1-f);
    }
    else if(amp3 >= 5./5.)
    {
    	f = (amp3 - 5./5.) * 5;
    	color_3.rgb = vec3(f,1-f,1-f);
    }
    
    double fraction = 0.0f;
    if (iterations < color_ranges[1])
    {
        fraction = (iterations - color_ranges[0]) / (color_ranges[1] - color_ranges[0]);
        return mix(color_0, color_1, float(fraction));
    }
    else if(iterations < color_ranges[2])
    {
        fraction = (iterations - color_ranges[1]) / (color_ranges[2] - color_ranges[1]);
        return mix(color_1, color_2, float(fraction));
    }
    else
    {
        fraction = (iterations - color_ranges[2]) / (color_ranges[3] - color_ranges[2]);
        return mix(color_2, color_3, float(fraction));
    }
}

void main()
{
//float f;
//if(amp < 1./5.)
//{
//	f = (amp/5) * 5;
//	color.rgb = vec3(f, 0, 0);
//}
//else if(amp < 2./5.)
//{
//	f = (amp - 1./5.) * 5;
//	color.rgb = vec3(1,f,0);
//}
//else if(amp < 3./5.)
//{
//	f = (amp - 2./5.) * 5;
//	color.rgb = vec3(0,f,0);
//}
//else if(amp < 4./5.)
//{
//	f = (amp - 3./5.) * 5;
//	color.rgb = vec3(1-f,f,0);
//}
//else if(amp < 5./5.)
//{
//	f = (amp - 4./5.) * 5;
//	color.rgb = vec3(f,1,1-f);
//}
//else if(amp >= 5./5.)
//{
//	f = (amp - 5./5.) * 5;
//	color.rgb = vec3(1-f,f,1-f);
//}
//
//if(colorext.g != 0)
//	color.rgb += pos;
//color.rgb = vec3(amp, 0.4 * amp, 0.1 * amp);
 color = return_color();
}
//float f = colors;
//if(audio<1./5.)
//{
//f = audio*5;
//color = vec3(f,0,0);
//}
//else if(audio<2./5.)
//{
//f = (audio-1./5.)*5;
//color = vec3(1,f,0);
//}
//else if(audio<3./5.)
//{
//f = (audio-2./5.)*5;
//color = vec3(1-f,1,f);
//}
//