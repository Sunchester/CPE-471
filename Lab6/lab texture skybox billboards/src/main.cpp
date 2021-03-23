/*
CPE/CSC 471 Lab base code Wood/Dunn/Eckhardt
*/

#include <iostream>
#include <glad/glad.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "GLSL.h"
#include "Program.h"
#include "MatrixStack.h"

#include "WindowManager.h"
#include "Shape.h"
// value_ptr for glm
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>
using namespace std;
using namespace glm;
shared_ptr<Shape> shape;


double get_last_elapsed_time()
{
	static double lasttime = glfwGetTime();
	double actualtime =glfwGetTime();
	double difference = actualtime- lasttime;
	lasttime = actualtime;
	return difference;
}
vector <vec3> posV, posV2, posV3;
float RandomNumber(int Min, int Max)
{
	return (Min + rand() % (Max - Min + 1));
}

vec3 campos;
bool compare_fct(vec3& i, vec3& j)
{
	float dist1 = distance(campos, i);
	float dist2 = distance(campos, j);
	return dist1 > dist2;
}

class camera
{
public:
	glm::vec3 pos, rot;
	int w, a, s, d, t, g,r,f;
	camera()
	{
		w = a = s = d = t = g = r= f= 0;
		pos = rot = glm::vec3(0, 25, -100);
	}
	glm::mat4 process(double ftime)
	{
		float speed = 0;
		float updown = 0;
		if (w == 1)
		{
			speed = 15*ftime;
		}
		else if (s == 1)
		{
			speed = -15*ftime;
		}
		if (t == 1)
		{
			updown = -15 * ftime;
		}
		else if (g == 1)
		{
			updown = 15 * ftime;
		}
		float xangle = 0;
		if (r == 1)
		{
			xangle = -3 * ftime;
		}
		else if (f == 1)
		{
			xangle = 3 * ftime;
		}
		float yangle=0;
		if (a == 1)
			yangle = -3*ftime;
		else if(d==1)
			yangle = 3*ftime;
		rot.y += yangle;
		rot.x += xangle;
		glm::mat4 R = glm::rotate(glm::mat4(1), rot.y, glm::vec3(0, 1, 0));
		glm::mat4 Rx = glm::rotate(glm::mat4(1), rot.x, glm::vec3(1, 0, 0));
		glm::vec4 dir = glm::vec4(0, updown, speed,1);
		dir = dir*Rx*R;
		pos += glm::vec3(dir.x, dir.y, dir.z);
		glm::mat4 T = glm::translate(glm::mat4(1), pos);
		return Rx*R*T;
	}
};

camera mycam;

class Application : public EventCallbacks
{

public:

	WindowManager * windowManager = nullptr;

	// Our shader program
	std::shared_ptr<Program> prog,psky;

	// Contains vertex information for OpenGL
	GLuint VertexArrayID;

	// Data necessary to give our box to OpenGL
	GLuint VertexBufferID, VertexNormDBox, VertexTexBox, IndexBufferIDBox;

	//texture data
	GLuint TextureClouds;
	GLuint Texture2;

	void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
	{
		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		{
			glfwSetWindowShouldClose(window, GL_TRUE);
		}
		
		if (key == GLFW_KEY_W && action == GLFW_PRESS)
		{
			mycam.w = 1;
		}
		if (key == GLFW_KEY_W && action == GLFW_RELEASE)
		{
			mycam.w = 0;
		}
		if (key == GLFW_KEY_S && action == GLFW_PRESS)
		{
			mycam.s = 1;
		}
		if (key == GLFW_KEY_S && action == GLFW_RELEASE)
		{
			mycam.s = 0;
		}
		if (key == GLFW_KEY_A && action == GLFW_PRESS)
		{
			mycam.a = 1;
		}
		if (key == GLFW_KEY_A && action == GLFW_RELEASE)
		{
			mycam.a = 0;
		}
		if (key == GLFW_KEY_D && action == GLFW_PRESS)
		{
			mycam.d = 1;
		}
		if (key == GLFW_KEY_D && action == GLFW_RELEASE)
		{
			mycam.d = 0;
		}
		if (key == GLFW_KEY_T && action == GLFW_PRESS)
		{
			mycam.t = 1;
		}
		if (key == GLFW_KEY_T && action == GLFW_RELEASE)
		{
			mycam.t = 0;
		}
		if (key == GLFW_KEY_G && action == GLFW_PRESS)
		{
			mycam.g = 1;
		}
		if (key == GLFW_KEY_G && action == GLFW_RELEASE)
		{
			mycam.g = 0;
		}
		if (key == GLFW_KEY_R && action == GLFW_PRESS)
		{
			mycam.r = 1;
		}
		if (key == GLFW_KEY_R && action == GLFW_RELEASE)
		{
			mycam.r = 0;
		}
		if (key == GLFW_KEY_F && action == GLFW_PRESS)
		{
			mycam.f = 1;
		}
		if (key == GLFW_KEY_F && action == GLFW_RELEASE)
		{
			mycam.f = 0;
		}
	}

	// callback for the mouse when clicked move the triangle when helper functions
	// written
	void mouseCallback(GLFWwindow *window, int button, int action, int mods)
	{
		double posX, posY;
		float newPt[2];
		if (action == GLFW_PRESS)
		{
			glfwGetCursorPos(window, &posX, &posY);
			std::cout << "Pos X " << posX <<  " Pos Y " << posY << std::endl;

			//change this to be the points converted to WORLD
			//THIS IS BROKEN< YOU GET TO FIX IT - yay!
			newPt[0] = 0;
			newPt[1] = 0;

			std::cout << "converted:" << newPt[0] << " " << newPt[1] << std::endl;
			glBindBuffer(GL_ARRAY_BUFFER, VertexBufferID);
			//update the vertex array with the updated points
			glBufferSubData(GL_ARRAY_BUFFER, sizeof(float)*6, sizeof(float)*2, newPt);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
		}
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
		glGenBuffers(1, &VertexBufferID);
		//set the current state to focus on our vertex buffer
		glBindBuffer(GL_ARRAY_BUFFER, VertexBufferID);

		GLfloat rect_vertices[] = {
			// front
			-1.0, -1.0,  1.0,//LD
			1.0, -1.0,  1.0,//RD
			1.0,  1.0,  1.0,//RU
			-1.0,  1.0,  1.0,//LU
		};
		//make it a bit smaller
		for (int i = 0; i < 12; i++)
			rect_vertices[i] *= 0.5;
		//actually memcopy the data - only do this once
		glBufferData(GL_ARRAY_BUFFER, sizeof(rect_vertices), rect_vertices, GL_DYNAMIC_DRAW);

		//we need to set up the vertex array
		glEnableVertexAttribArray(0);
		//key function to get up how many elements to pull out at a time (3)
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		//color
		GLfloat cube_norm[] = {
			// front colors
			0.0, 0.0, 1.0,
			0.0, 0.0, 1.0,
			0.0, 0.0, 1.0,
			0.0, 0.0, 1.0,

		};
		glGenBuffers(1, &VertexNormDBox);
		//set the current state to focus on our vertex buffer
		glBindBuffer(GL_ARRAY_BUFFER, VertexNormDBox);
		glBufferData(GL_ARRAY_BUFFER, sizeof(cube_norm), cube_norm, GL_STATIC_DRAW);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		//color
		glm::vec2 cube_tex[] = {
			// front colors
			glm::vec2(0.0, 1.0),
			glm::vec2(1.0, 1.0),
			glm::vec2(1.0, 0.0),
			glm::vec2(0.0, 0.0),

		};
		glGenBuffers(1, &VertexTexBox);
		//set the current state to focus on our vertex buffer
		glBindBuffer(GL_ARRAY_BUFFER, VertexTexBox);
		glBufferData(GL_ARRAY_BUFFER, sizeof(cube_tex), cube_tex, GL_STATIC_DRAW);
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

		glGenBuffers(1, &IndexBufferIDBox);
		//set the current state to focus on our vertex buffer
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndexBufferIDBox);
		GLushort cube_elements[] = {

			// front
			0, 1, 2,
			2, 3, 0,
		};
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_elements), cube_elements, GL_STATIC_DRAW);



		glBindVertexArray(0);

		string resourceDirectory = "../resources" ;
		// Initialize mesh.
		shape = make_shared<Shape>();
		//shape->loadMesh(resourceDirectory + "/t800.obj");
		shape->loadMesh(resourceDirectory + "/sphere.obj");
		shape->resize();
		shape->init();

		int width, height, channels;
		char filepath[1000];

		////texture 1
		//string str = resourceDirectory + "/grass.jpg";
		//strcpy(filepath, str.c_str());
		//unsigned char* data = stbi_load(filepath, &width, &height, &channels, 4);
		//glGenTextures(1, &Texture);
		////glActiveTexture(GL_TEXTURE0);
		//glBindTexture(GL_TEXTURE_2D, Texture);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		//glGenerateMipmap(GL_TEXTURE_2D);

		////texture Night
		//str = resourceDirectory + "/night.jpg";
		//strcpy(filepath, str.c_str());
		//data = stbi_load(filepath, &width, &height, &channels, 4);
		//glGenTextures(1, &TextureN);
		////glActiveTexture(GL_TEXTURE0);
		//glBindTexture(GL_TEXTURE_2D, TextureN);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		//glGenerateMipmap(GL_TEXTURE_2D);





		//texture 2
		string str = resourceDirectory + "/dep_sky.jpg";
		strcpy(filepath, str.c_str());
		unsigned char* data = stbi_load(filepath, &width, &height, &channels, 4);
		glGenTextures(1, &Texture2);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, Texture2);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		str = resourceDirectory + "/clouds.jpg";
		strcpy(filepath, str.c_str());
		data = stbi_load(filepath, &width, &height, &channels, 4);
		glGenTextures(1, &TextureClouds);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, TextureClouds);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
		//[TWOTEXTURES]
		//set the 2 textures to the correct samplers in the fragment shader:
		GLuint Tex1Location = glGetUniformLocation(prog->pid, "tex");//tex, tex2... sampler in the fragment shader
		// Then bind the uniform samplers to texture units:
		glUseProgram(prog->pid);
		glUniform1i(Tex1Location, 0);

		Tex1Location = glGetUniformLocation(psky->pid, "tex");//tex, tex2... sampler in the fragment shader
		// Then bind the uniform samplers to texture units:
		glUseProgram(psky->pid);
		glUniform1i(Tex1Location, 0);

	}

	//General OGL initialization - set OGL state here
	void init(const std::string& resourceDirectory)
	{
		GLSL::checkVersion();

		// Set background color.
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		// Enable z-buffer test.
		glEnable(GL_DEPTH_TEST);
		//glDisable(GL_DEPTH_TEST);
		// Initialize the GLSL program.
		prog = std::make_shared<Program>();
		prog->setVerbose(true);
		prog->setShaderNames(resourceDirectory + "/shader_vertex.glsl", resourceDirectory + "/shader_fragment.glsl");
		if (!prog->init())
		{
			std::cerr << "One or more shaders failed to compile... exiting!" << std::endl;
			exit(1);
		}
		prog->addUniform("P");
		prog->addUniform("V");
		prog->addUniform("M");
		prog->addAttribute("vertPos");
		prog->addAttribute("vertTex");
		prog->addUniform("offset");



		psky = std::make_shared<Program>();
		psky->setVerbose(true);
		psky->setShaderNames(resourceDirectory + "/skyvertex.glsl", resourceDirectory + "/skyfrag.glsl");
		if (!psky->init())
		{
			std::cerr << "One or more shaders failed to compile... exiting!" << std::endl;
			exit(1);
		}
		psky->addUniform("P");
		psky->addUniform("V");
		psky->addUniform("M");
		psky->addAttribute("vertPos");
		psky->addAttribute("vertTex");

		auto iterator = posV.begin();
		for (int x = 0; x < 20; x++)
		{
			iterator = posV.insert(iterator,vec3(RandomNumber(-7, 90), RandomNumber(-5, 40), RandomNumber(-115, -185)));
			
		}
		
		/*int j = 0;
		for(vec3 po: posV)
		{
			j += 1;
			cout << "X COMPONENT: " << po.x << "Y COMPONENT: " << po.y << "Z COMPONENT: " << po.z << endl;
		}*/
	}

	
	/****DRAW
	This is the most important function in your program - this is where you
	will actually issue the commands to draw any geometry you have set up to
	draw
	********/
	
	void render()
	{
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		double frametime = get_last_elapsed_time();
		static int x = 0;
		static int y = 0;
		// Get current frame buffer size.
		int width, height;
		glfwGetFramebufferSize(windowManager->getHandle(), &width, &height);
		float aspect = width/(float)height;
		glViewport(0, 0, width, height);

		// Clear framebuffer.
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Create the matrix stacks - please leave these alone for now
		
		glm::mat4 V, M, P; //View, Model and Perspective matrix
		V = mycam.process(frametime);
		M = glm::mat4(1);
		P = glm::perspective((float)(3.14159 / 4.), (float)((float)width/ (float)height), 0.1f, 1000.0f); //so much type casting... GLM metods are quite funny ones
		
		float sangle = 3.1415926 / 2.;
		glm::mat4 RotateXSky = glm::rotate(glm::mat4(1.0f), sangle, glm::vec3(1.0f, 0.0f, 0.0f));
		glm::vec3 camp = -mycam.pos;
		campos = -mycam.pos;
		glm::mat4 TransSky = glm::translate(glm::mat4(1.0f), camp);
		glm::mat4 SSky = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, 1.0f));

		M = TransSky * RotateXSky * SSky;


		psky->bind();		
		glUniformMatrix4fv(psky->getUniform("P"), 1, GL_FALSE, &P[0][0]);
		glUniformMatrix4fv(psky->getUniform("V"), 1, GL_FALSE, &V[0][0]);
		glUniformMatrix4fv(psky->getUniform("M"), 1, GL_FALSE, &M[0][0]);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, Texture2);	
		glDisable(GL_DEPTH_TEST);
		shape->draw(psky, FALSE);
		glEnable(GL_DEPTH_TEST);	
		psky->unbind();

		
		glm::mat4 RotateX;
		glm::mat4 TransZ;
		glm::mat4 S;
 
	
		// Draw the box using GLSL.
		prog->bind();

	
		glBindVertexArray(VertexArrayID);
		//actually draw from vertex 0, 3 vertices
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndexBufferIDBox);
		//glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, (void*)0);
		
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, TextureClouds);
		glDisable(GL_DEPTH_TEST);
		mat4 particleView = V;
		particleView[3][0] = 0;
		particleView[3][1] = 0;
		particleView[3][2] = 0;
		particleView = inverse(particleView);
		
		
		
		sort(posV.begin(), posV.end(), (compare_fct));

		for (vec3 c : posV)
		{

			TransZ = glm::translate(glm::mat4(1.0f), c);
			S = glm::scale(glm::mat4(1.0f), glm::vec3(24.f, 24.f, 0.f));
			M = TransZ * particleView * S;
			vec2 off = vec2(2, 3) * 0.25f;
			glUniformMatrix4fv(prog->getUniform("P"), 1, GL_FALSE, &P[0][0]);
			glUniformMatrix4fv(prog->getUniform("V"), 1, GL_FALSE, &V[0][0]);
			glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, &M[0][0]);
			glUniform2fv(prog->getUniform("offset"), 1, &off.x);
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, (void*)0);

		}
		for (vec3 c : posV)
		{

			TransZ = glm::translate(glm::mat4(1.0f), c);
			S = glm::scale(glm::mat4(1.0f), glm::vec3(24.f, 24.f, 0.f));
			M = TransZ * particleView * S;
			vec2 off = vec2(2, 2) * 0.25f;
			glUniformMatrix4fv(prog->getUniform("P"), 1, GL_FALSE, &P[0][0]);
			glUniformMatrix4fv(prog->getUniform("V"), 1, GL_FALSE, &V[0][0]);
			glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, &M[0][0]);
			glUniform2fv(prog->getUniform("offset"), 1, &off.x);
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, (void*)0);

		}

		for (vec3 c : posV)
		{

			TransZ = glm::translate(glm::mat4(1.0f), c);
			S = glm::scale(glm::mat4(1.0f), glm::vec3(24.f, 24.f, 0.f));
			M = TransZ * particleView * S;
			vec2 off = vec2(3, 2) * 0.25f;
			glUniformMatrix4fv(prog->getUniform("P"), 1, GL_FALSE, &P[0][0]);
			glUniformMatrix4fv(prog->getUniform("V"), 1, GL_FALSE, &V[0][0]);
			glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, &M[0][0]);
			glUniform2fv(prog->getUniform("offset"), 1, &off.x);
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, (void*)0);

		}



		
		glEnable(GL_DEPTH_TEST);

		glBindVertexArray(0);
		
		prog->unbind();

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

	// Quit program.
	windowManager->shutdown();
	return 0;
}
