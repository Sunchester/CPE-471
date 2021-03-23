/*
musicvisualizer base code
by Christian Eckhardt 2018
with some code snippets from Ian Thomas Dunn and Zoe Wood, based on their CPE CSC 471 base code
On Windows, it whould capture "what you here" automatically, as long as you have the Stereo Mix turned on!! (Recording devices -> activate)
*/

#include <iostream>
#include <glad/glad.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "GLSL.h"
#include "Program.h"
#include "MatrixStack.h"
#include "recordAudio.h"
#include "WindowManager.h"
#include "Shape.h"
// value_ptr for glm
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <thread>
using namespace std;
using namespace glm;
shared_ptr<Shape> shape, shape2;
#define MESHSIZE 1000
extern captureAudio actualAudioData;
extern int running;

int renderstate = 1;//2..grid

#define TEXSIZE 256
BYTE texels[TEXSIZE*TEXSIZE*4];
//********************
#include <math.h>
#include <algorithm>    


#include "kiss_fft.h"



#define FFTW_ESTIMATEE (1U << 6)
#define FFT_MAXSIZE 500

bool fft(float *amplitude_on_frequency,int &length)
{


	int N = pow(2, 10);
	BYTE data[MAXS];
	int size = 0;
	actualAudioData.readAudio(data, size);
	length = size / 8;
	if (size == 0)
		return false;

	double *samples = new double[length];
	for (int ii = 0; ii < length; ii++)
	{
		float *f = (float*)&data[ii * 8];
		samples[ii] = (double)(*f);
	}
	

	kiss_fft_cpx *cx_in = new kiss_fft_cpx[length];
	kiss_fft_cpx *cx_out = new kiss_fft_cpx[length];
	kiss_fft_cfg cfg = kiss_fft_alloc(length, 0, 0, 0);
	for (int i = 0; i<length; ++i) 
		{		
		cx_in[i].r = samples[i];
		cx_in[i].i = 0;
		}

		kiss_fft(cfg, cx_in, cx_out);

		float amplitude_on_frequency_old[FFT_MAXSIZE];
		for (int i = 0; i < length / 2 && i<FFT_MAXSIZE; ++i)
			amplitude_on_frequency_old[i] = amplitude_on_frequency[i];

		for (int i = 0; i < length/2 && i<FFT_MAXSIZE; ++i)
			amplitude_on_frequency[i] = sqrt(pow(cx_out[i].i,2.) + pow(cx_out[i].r, 2.));


		//that looks better, decomment for no filtering: +++++++++++++++++++
		for (int i = 0; i < length / 2 && i < FFT_MAXSIZE; ++i)
				{
				float diff = amplitude_on_frequency_old[i] - amplitude_on_frequency[i];
				float attack_factor = 0.1;//for going down
				if (amplitude_on_frequency_old[i] < amplitude_on_frequency[i]) 
					attack_factor = 0.85; //for going up
				diff *= attack_factor;
				amplitude_on_frequency[i] = amplitude_on_frequency_old[i] - diff;
				}			
		//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


		length /= 2;
		free(cfg);

	return true;
}
#define min(a,b) ((a) < (b) ? (a) : (b))
#define max(a,b) ((a) > (b) ? (a) : (b))

BYTE delayfilter(BYTE old, BYTE actual, float mul)
{
	float fold = (float)old;
	float factual = (float)actual;
	float fres = fold - (fold - factual) / mul;
	if (fres > 255) fres = 255;
	else if (fres < 0)fres = 0;
	return (BYTE)fres;
}

double get_last_elapsed_time()
{
	static double lasttime = glfwGetTime();
	double actualtime =glfwGetTime();
	double difference = actualtime- lasttime;
	lasttime = actualtime;
	return difference;
}
class camera
{
public:
	glm::vec3 pos, rot;
	int w, a, s, d, q, e, z, c;
	camera()
	{
		w = a = s = d = q = e = z = c = 0;
		pos = rot = glm::vec3(0, 0, 0);
	}
	glm::mat4 process(double ftime)
	{
		float speed = 0;
		if (w == 1)
		{
			speed = 90*ftime;
		}
		else if (s == 1)
		{
			speed = -90*ftime;
		}
		float yangle=0;
		if (a == 1)
			yangle = -3*ftime;
		else if(d==1)
			yangle = 3*ftime;
		rot.y += yangle;
		float zangle = 0;
		if (q == 1)
			zangle = -3 * ftime;
		else if (e == 1)
			zangle = 3 * ftime;
		rot.z += zangle;
		float xangle = 0;
		if (z == 1)
			xangle = -0.3 * ftime;
		else if (c == 1)
			xangle = 0.3 * ftime;
		rot.x += xangle;

		glm::mat4 R = glm::rotate(glm::mat4(1), rot.y, glm::vec3(0, 1, 0));
		glm::mat4 Rz = glm::rotate(glm::mat4(1), rot.z, glm::vec3(0, 0, 1));
		glm::mat4 Rx = glm::rotate(glm::mat4(1), rot.x, glm::vec3(1, 0, 0));
		glm::vec4 dir = glm::vec4(0, 0, speed,1);
		R = Rx * Rz * R;
		dir = dir*R;
		pos += glm::vec3(dir.x, dir.y, dir.z);
		glm::mat4 T = glm::translate(glm::mat4(1), pos);
		return R*T;
	}
};

camera mycam;
float center_x{ 0.0f };
float center_y{ 0.0f };
float zoom{ 1.0 };
int screen_width{ 1920 };
int screen_height{ 1080 };
class Application : public EventCallbacks
{

public:

	WindowManager * windowManager = nullptr;

	// Our shader program
	std::shared_ptr<Program> prog;

	// Contains vertex information for OpenGL
	GLuint VertexArrayID, FractArrayID, FractBuff, FractIND;

	// Data necessary to give our box to OpenGL
	GLuint MeshPosID, MeshTexID;
	// FFT arrays
	float amplitude_on_frequency[FFT_MAXSIZE];
	float amplitude_on_frequency_10steps[10];

	int move = 0;
	int up = 0;
	int down = 0;
	int left = 0;
	int right = 0;
	int zoomout = 0;
	int zoomin = 0;
	void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
	{
		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		{
			glfwSetWindowShouldClose(window, GL_TRUE);
		}
		if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
		{
			up = 1;
		}
		if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_RELEASE)
		{
			up = 0;
		}
		if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
		{
			down = 1;

		}
		if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_RELEASE)
		{
			down = 0;

		}


		if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
		{
			left = 1;
		}
		if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_RELEASE)
		{
			left = 0;
		}

		if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
		{
			right = 1;
		}
		if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_RELEASE)
		{
			right = 0;
		}
		
		if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		{
			zoomout = 1;
		}
		if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE)
		{
			zoomout = 0;
		}

		
		if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
		{
			zoomin = 1;
		}
		if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_RELEASE)
		{
			zoomin = 0;
		}
		
		if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS)
		{
			move = 1;
		}
		if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS)
		{
			move = 0;
		}
	
		
	}

	glm::vec4 find_ranges(std::vector<float>& data)
	{
		float lowest = 10000;
		float highest = 0;
		for (int i = 0; i < data.size(); i += 20) //or i++, or i+=10, whatever is acceptable
		{
			if (data[i] > 0.0 && data[i] < lowest)
				lowest = data[i];
			if (data[i] > highest)
				highest = data[i];
		}
		int size = data.size();
		int length = size - lowest;
		float m = (lowest + highest) / 2.;
		float stroke = highest - m;
		glm::vec4 ranges = glm::vec4(m - stroke, m-stroke*0.3, m + stroke * 0.3, m + stroke);
		return ranges;
	}
	// callback for the mouse when clicked move the triangle when helper functions
	// written
	void mouseCallback(GLFWwindow *window, int button, int action, int mods)
	{
	
	}

	//if the window is resized, capture the new size and reset the viewport
	void resizeCallback(GLFWwindow *window, int in_width, int in_height)
	{
		//get the window size - may be different then pixels for retina
		int width, height;
		glfwGetFramebufferSize(window, &width, &height);
		glViewport(0, 0, width, height);
	}

	/*Note that any gl calls must always happen after a GL state is initialized */
	void initGeom()
	{
		//generate the VAO
		glGenVertexArrays(1, &VertexArrayID);
		glBindVertexArray(VertexArrayID);

		//generate vertex buffer to hand off to OGL
		glGenBuffers(1, &MeshPosID);
		glBindBuffer(GL_ARRAY_BUFFER, MeshPosID);
		vec3 vertices[FFT_MAXSIZE];
		float steps = 10. / (float)FFT_MAXSIZE;

		for (int i = 0; i < FFT_MAXSIZE; i++)
			vertices[i] = vec3(-5. + (float)i*steps , 0.0, 0.0);
				
		glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * FFT_MAXSIZE, vertices, GL_DYNAMIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
		
		
		// Fractal Geo Initialization

		glGenVertexArrays(1, &FractArrayID);
		glBindVertexArray(FractArrayID);

		glGenBuffers(1, &FractBuff);

		glBindBuffer(GL_ARRAY_BUFFER, FractBuff);

		GLfloat fractVert[] =
		{
			-1.0f, -1.0f, 0.0f,
			 1.0f,  1.0f, 0.0f,
			-1.0f,  1.0f, 0.0f,
			 1.0f, -1.0f, 0.0f
		};
		glBufferData(GL_ARRAY_BUFFER, sizeof(fractVert), fractVert, GL_DYNAMIC_DRAW);
		
		
		
		
		glGenBuffers(1, &FractIND);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, FractIND);
		GLushort fractInd[] =
		{
			0, 1, 2,
			0, 3, 1
		};
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(fractInd), fractInd, GL_DYNAMIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glBindVertexArray(0);




	}
	
	//General OGL initialization - set OGL state here
	void init(const std::string& resourceDirectory)
	{
		

		GLSL::checkVersion();

		// Set background color.
		glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
		// Enable z-buffer test.
		glEnable(GL_DEPTH_TEST);
		
		// Initialize the GLSL program.
		prog = std::make_shared<Program>();
		prog->setVerbose(true);
		prog->setShaderNames(resourceDirectory + "/shader_vertex.glsl", resourceDirectory + "/shader_fragment.glsl");
		if (!prog->init())
		{
			std::cerr << "One or more shaders failed to compile... exiting!" << std::endl;
			exit(1);
		}
		prog->addUniform("amp");
		prog->addUniform("amp2");
		prog->addUniform("amp3");
		prog->addUniform("amp4");

		prog->addUniform("center_x");
		prog->addUniform("center_y");
		prog->addUniform("zoom");
		prog->addUniform("color_ranges");
		prog->addUniform("movement");
		prog->addAttribute("vertPos");

		
		
	}
//*******************
	void aquire_fft_scaling_arrays()
		{
		//get FFT array
		static int length = 0;
		if (fft(amplitude_on_frequency, length))
			{
			//put the height of the frequencies 20Hz to 20000Hz into the height of the line-vertices
			vec3 vertices[FFT_MAXSIZE];
			glBindBuffer(GL_ARRAY_BUFFER, MeshPosID);
			float steps = 10. / (float)FFT_MAXSIZE;
			for (int i = 0; i < FFT_MAXSIZE; i++)
				{
				float step = i / (float)length;
				step *= 20;

				float height = 0;
				if (i < length)
					height = amplitude_on_frequency[i] * 0.05 * (1+step);
				vertices[i] = vec3(-5. + (float)i*steps, height, 0.0);
				}
			glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vec3) * FFT_MAXSIZE, vertices);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			//calculate the average amplitudes for the 10 spheres
			for (int i = 0; i < 10; i++)
				amplitude_on_frequency_10steps[i] = 0;

			int mean_range = length / 10;
			int bar = 0;
			int count = 0;
			
			for (int i = 0; ; i++, count++)
				{
				if (mean_range == count)
					{
					count = -1;
					amplitude_on_frequency_10steps[bar] /= (float)mean_range;
					bar++;
					if (bar == 10)break;
					}
				if (i<length && i<FFT_MAXSIZE)
					amplitude_on_frequency_10steps[bar] += amplitude_on_frequency[i];
				}
			}
		}

	/****DRAW
	This is the most important function in your program - this is where you
	will actually issue the commands to draw any geometry you have set up to
	draw
	********/
	void render()
	{
		static glm::vec4 ranges = glm::vec4(0.0001f, 0.33333f, 0.66667f, 1.00f);
		static std::vector<float> pixel_data(screen_width * screen_height, 0.0f);
		static double count = 0;
		double frametime = get_last_elapsed_time();
		count += frametime;
		
		// Get current frame buffer size.
		int width, height;
		glfwGetFramebufferSize(windowManager->getHandle(), &width, &height);
		float aspect = width/(float)height;
		glViewport(0, 0, width, height);

		// Clear framebuffer.
		glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm::mat4 V, M, P; //View, Model and Perspective matrix
		
		V = mycam.process(frametime);
		P = glm::perspective((float)(3.14159 / 4.), (float)((float)width/ (float)height), 0.01f, 100000.0f); //so much type casting... GLM metods are quite funny ones
				
		
		// Draw the box using GLSL.
		prog->bind();
		
		
		//Set the FFT arrays
		aquire_fft_scaling_arrays();


		if (up == 1)
		{
			center_y = center_y + 0.005f * zoom;
			if (center_y > 1.0f)
			{
				center_y = 1.0f;
			}
		}
		if (down == 1)
		{
			center_y = center_y - 0.005f * zoom;
			if (center_y < -1.0f)
			{
				center_y = -1.0f;
			}
		}
		if (left == 1)
		{
			center_x = center_x - 0.005f * zoom;
			if (center_x < -1.0f)
			{
				center_x = -1.0f;
			}
		}
		if (right == 1)
		{
			center_x = center_x + 0.005f * zoom;
			if (center_x > 1.0f)
			{
				center_x = 1.0f;
			}
		}

		if (zoomout == 1)
		{
			zoom = zoom * 1.02f;
			if (zoom > 1.0f)
			{
				zoom = 1.0f;
			}
		}

		if (zoomin == 1)
		{
			zoom = zoom * 0.98f;
			if (zoom < 0.00001f)
			{
				zoom = 0.00001f;
			}
		}
		static float ttime = 0;
		ttime += frametime*0.005;
		float dn = sin(ttime) *0.5 + 0.5;
		glUniform1f(prog->getUniform("amp"), amplitude_on_frequency_10steps[0]/10.0f);
		glUniform1f(prog->getUniform("amp2"), amplitude_on_frequency_10steps[1]/3.3f);
		glUniform1f(prog->getUniform("amp3"), amplitude_on_frequency_10steps[2]/1.50f); 
		glUniform1f(prog->getUniform("amp4"), dn);
		glUniform1f(prog->getUniform("center_x"), center_x);
		glUniform1f(prog->getUniform("center_y"), center_y);
		glUniform1f(prog->getUniform("zoom"), zoom);
		glUniform4fv(prog->getUniform("color_ranges"), 1, &ranges[0]);
		glUniform1i(prog->getUniform("movement"), move);
		glBindVertexArray(FractArrayID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, FractIND);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);
		
		glReadPixels(0, 0, screen_width, screen_height, GL_DEPTH_COMPONENT, GL_FLOAT, pixel_data.data());
		ranges = find_ranges(pixel_data);

		prog->unbind();
		glBindVertexArray(0);

		}
	

};
//******************************************************************************************
int main(int argc, char **argv)
{
	std::string resourceDir = "../resources"; // Where the resources are loaded from
	if (argc >= 2)
	{
		resourceDir = argv[1];
	}

	Application *application = new Application();

	/* your main will always include a similar set up to establish your window
		and GL context, etc. */
	WindowManager * windowManager = new WindowManager();
	windowManager->init(1920, 1080);
	windowManager->setEventCallbacks(application);
	application->windowManager = windowManager;

	/* This is the code that will likely change program to program as you
		may need to initialize or set up different data and state */
	// Initialize scene.
	application->init(resourceDir);
	application->initGeom();

	thread t1(start_recording);
	// Loop until the user closes the window.
	while(! glfwWindowShouldClose(windowManager->getHandle()))
	{
		// Render scene.
		application->render();
		// Swap front and back buffers.
		glfwSwapBuffers(windowManager->getHandle());
		// Poll for and process events.
		glfwPollEvents();
	}
	running = FALSE;
	t1.join();

	// Quit program.
	windowManager->shutdown();
	return 0;
}
