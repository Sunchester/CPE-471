/*
ZJ Wood, Dunn, Eckhardt CPE 471 Lab base code
*/

#include <iostream>
#include <glad/glad.h>

#include "GLSL.h"
#include "Program.h"
#include "MatrixStack.h"

#include "WindowManager.h"
#include "Shape.h"
// value_ptr for glm
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace std;
using namespace glm;
Shape shape;


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
	int w, a, s, d;
	camera()
	{
		w = a = s = d = 0;
		pos = rot = glm::vec3(0, 0, 0);
	}
	glm::mat4 process(double ftime)
	{
		float speed = 0;
		if (w == 1)
		{
			speed = 1*ftime;
		}
		else if (s == 1)
		{
			speed = -1*ftime;
		}
		float yangle=0;
		if (a == 1)
			yangle = -1*ftime;
		else if(d==1)
			yangle = 1*ftime;
		rot.y += yangle;
		glm::mat4 R = glm::rotate(glm::mat4(1), rot.y, glm::vec3(0, 1, 0));
		glm::vec4 dir = glm::vec4(0, 0, speed,1);
		dir = dir*R;
		pos += glm::vec3(dir.x, dir.y, dir.z);
		glm::mat4 T = glm::translate(glm::mat4(1), pos);
		return R*T;
	}
};

camera mycam;

class Application : public EventCallbacks
{

public:

	WindowManager * windowManager = nullptr;

	// Our shader program
	std::shared_ptr<Program> prog;

	// Contains vertex information for OpenGL
	GLuint VertexArrayID;

	// Data necessary to give our box to OpenGL
	GLuint VertexBufferID, VertexColorIDBox, IndexBufferIDBox;

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
			std::cout << "Pos X " << posX << " Pos Y " << posY << std::endl;

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
		string resourceDirectory = "../resources" ;
		// Initialize mesh.
	
		shape.loadMesh(resourceDirectory + "/t800.obj");
		shape.resize();
		shape.init();

		//generate the VAO
		glGenVertexArrays(1, &VertexArrayID);
		glBindVertexArray(VertexArrayID);

		//generate vertex buffer to hand off to OGL
		glGenBuffers(1, &VertexBufferID);
		//set the current state to focus on our vertex buffer
		glBindBuffer(GL_ARRAY_BUFFER, VertexBufferID);

		GLfloat cube_vertices[] = {
			// front
			-1.0, -1.0,  1.0,
			1.0, -1.0,  1.0,
			1.0,  1.0,  1.0,
			-1.0,  1.0,  1.0,
			// back
			-1.0, -1.0, -1.0,
			1.0, -1.0, -1.0,
			1.0,  1.0, -1.0,
			-1.0,  1.0, -1.0,
			//tube 8 - 11
			-1.0, -1.0,  1.0,
			1.0, -1.0,  1.0,
			1.0,  1.0,  1.0,
			-1.0,  1.0,  1.0,
			//12 - 15
			-1.0, -1.0, -1.0,
			1.0, -1.0, -1.0,
			1.0,  1.0, -1.0,
			-1.0,  1.0, -1.0


		};
		//actually memcopy the data - only do this once
		glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_DYNAMIC_DRAW);

		//we need to set up the vertex array
		glEnableVertexAttribArray(0);
		//key function to get up how many elements to pull out at a time (3)
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		//color
		GLfloat cube_colors[] = {
			// front colors
			1.0, 1.0, 1.0,
			1.0, 1.0, 1.0,
			1.0, 1.0, 1.0,
			1.0, 1.0, 1.0,
			// back colors
			1.0, 1.0, 1.0,
			1.0, 1.0, 1.0,
			1.0, 1.0, 1.0,
			1.0, 1.0, 1.0,
			// tube colors
			0.0, 1.0, 1.0,
			0.0, 1.0, 1.0,
			0.0, 1.0, 1.0,
			0.0, 1.0, 1.0,
			0.0, 1.0, 1.0,
			0.0, 1.0, 1.0,
			0.0, 1.0, 1.0,
			0.0, 1.0, 1.0,
		};
		glGenBuffers(1, &VertexColorIDBox);
		//set the current state to focus on our vertex buffer
		glBindBuffer(GL_ARRAY_BUFFER, VertexColorIDBox);
		glBufferData(GL_ARRAY_BUFFER, sizeof(cube_colors), cube_colors, GL_STATIC_DRAW);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		glGenBuffers(1, &IndexBufferIDBox);
		//set the current state to focus on our vertex buffer
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndexBufferIDBox);
		GLushort cube_elements[] = {

			// front
			0, 1, 2,
			2, 3, 0,
			// back
			7, 6, 5,
			5, 4, 7,
			//tube 8-11, 12-15
			8,12,13,
			8,13,9,
			9,13,14,
			9,14,10,
			10,14,15,
			10,15,11,
			11,15,12,
			11,12,8

		};
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_elements), cube_elements, GL_STATIC_DRAW);



		glBindVertexArray(0);

	}

	


	//General OGL initialization - set OGL state here
	void init(const std::string& resourceDirectory)
	{
		GLSL::checkVersion();

		// Set background color.
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		// Enable z-buffer test.
		glEnable(GL_DEPTH_TEST);

		// Initialize the GLSL program.
		prog = std::make_shared<Program>();
		prog->setVerbose(true);
		prog->setShaderNames(resourceDirectory + "/shader_vertex.glsl", resourceDirectory + "/shader_fragment.glsl");
		prog->init();
		prog->addUniform("P");
		prog->addUniform("V");
		prog->addUniform("M");
		prog->addAttribute("vertPos");
		
	}


	/****DRAW
	This is the most important function in your program - this is where you
	will actually issue the commands to draw any geometry you have set up to
	draw
	********/
	void render()
	{
		double frametime = get_last_elapsed_time();

		// Get current frame buffer size.
		int width, height;
		glfwGetFramebufferSize(windowManager->getHandle(), &width, &height);
		float aspect = width/(float)height;
		glViewport(0, 0, width, height);

		// Clear framebuffer.
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Create the matrix stacks - please leave these alone for now
		
		glm::mat4 V, M, P; //View, Model and Perspective matrix
		V = glm::mat4(1);
		M = glm::mat4(1);
		
		P = glm::perspective((float)(3.14159 / 4.), (float)((float)width/ (float)height), 0.1f, 1000.0f); //so much type casting... GLM metods are quite funny ones

		//animation with the model matrix:
		static float w = 0.0;
		w += 1.0 * frametime * 3;//rotation angle
		float trans = 0;// sin(t) * 2;
		glm::mat4 RotateY = glm::rotate(glm::mat4(1.0f), w, glm::vec3(0.0f, 1.0f, 0.0f));
		float angle = -3.1415926/2.0;

		/*glm::mat4 RotateX = glm::rotate(glm::mat4(1.0f), angle, glm::vec3(1.0f, 0.0f, 0.0f));
		glm::mat4 TransZ = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -3 + trans));
		glm::mat4 S = glm::scale(glm::mat4(1.0f), glm::vec3(0.8f, 0.8f, 0.8f));*/

		/*M =  TransZ * RotateY * RotateX * S;*/

		// Draw the box using GLSL.
		prog->bind();

		V = mycam.process(frametime);
		glBindVertexArray(VertexArrayID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndexBufferIDBox);

		mat4 rotAll = glm::rotate(mat4(1.0f), 0.261799f, vec3(0.0, 1.0, 0.0));
		// Middle Body of Snowman
		mat4 transl = glm::translate(mat4(1.0f), vec3(0.0f, 0.0, -3.5f));
		mat4 scale = glm::scale(mat4(1.0f), vec3(0.20f, 0.20f, 0.20f));
		M = transl * rotAll * scale;
		//send the matrices to the shaders
		glUniformMatrix4fv(prog->getUniform("P"), 1, GL_FALSE, &P[0][0]);
		glUniformMatrix4fv(prog->getUniform("V"), 1, GL_FALSE, &V[0][0]);
		glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, &M[0][0]);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, (void*)0);
		/*shape.draw(prog);	*/	

		// Head of Snowman
		mat4 translate1 = glm::translate(mat4(1.0f), vec3(0.0f, 0.355f, -3.5f));
		mat4 scale1 = glm::scale(mat4(1.0f), vec3(0.15f, 0.15f, 0.15f));
		M =  translate1* rotAll  * scale1;
		glUniformMatrix4fv(prog->getUniform("P"), 1, GL_FALSE, &P[0][0]);
		glUniformMatrix4fv(prog->getUniform("V"), 1, GL_FALSE, &V[0][0]);
		glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, &M[0][0]);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, (void*)0);


		// Bottom Body of Snowman
		mat4 translate2 = glm::translate(mat4(1.0f), vec3(0.0f, -0.45f, -3.5f));
		mat4 scale2 = glm::scale(mat4(1.0f), vec3(0.25f, 0.25f, 0.25f));
		M = translate2 * rotAll * scale2;
		glUniformMatrix4fv(prog->getUniform("P"), 1, GL_FALSE, &P[0][0]);
		glUniformMatrix4fv(prog->getUniform("V"), 1, GL_FALSE, &V[0][0]);
		glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, &M[0][0]);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, (void*)0);
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
