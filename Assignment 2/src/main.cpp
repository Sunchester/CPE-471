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
int moveStatus = 0;
int moveStatusR = 0;
int moveStatus2 = 0;
int moveStatus2R = 0;
class Application : public EventCallbacks
{

public:

	WindowManager * windowManager = nullptr;

	// Our shader program
	std::shared_ptr<Program> prog;
	std::shared_ptr<Program> prog2;
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
			moveStatus = -1;
		}
		if (key == GLFW_KEY_A && action == GLFW_RELEASE)
		{
			moveStatus = 0;
		}
		if (key == GLFW_KEY_D && action == GLFW_PRESS)
		{
			moveStatusR = 1;
		}
		if (key == GLFW_KEY_D && action == GLFW_RELEASE)
		{
			moveStatusR = 0;
		}
		if (key == GLFW_KEY_J && action == GLFW_PRESS)
		{
			moveStatus2 = -1;
		}
		if (key == GLFW_KEY_J && action == GLFW_RELEASE)
		{
			moveStatus2 = 0;
		}
		if (key == GLFW_KEY_L && action == GLFW_PRESS)
		{
			moveStatus2R = 1;
		}
		if (key == GLFW_KEY_L && action == GLFW_RELEASE)
		{
			moveStatus2R = 0;
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
			/*newPt[0] = 0;
			newPt[1] = 0;

			std::cout << "converted:" << newPt[0] << " " << newPt[1] << std::endl;
			glBindBuffer(GL_ARRAY_BUFFER, VertexBufferID);*/
			//update the vertex array with the updated points
			/*glBufferSubData(GL_ARRAY_BUFFER, sizeof(float)*6, sizeof(float)*2, newPt);
			glBindBuffer(GL_ARRAY_BUFFER, 0);*/
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
			0.0, 0.0, 0.0,
			0.0, 0.0, 0.0,
			0.0, 0.0, 0.0,
			0.0, 0.0, 0.0,
			// back colors
			0.5, 0.5, 0.0,
			0.5, 0.5, 0.0,
			0.5, 0.5, 0.0,
			0.5, 0.5, 0.0,
			// tube colors
			1.0, 1.0, 1.0,
			1.0, 1.0, 1.0,
			1.0, 1.0, 1.0,
			1.0, 1.0, 1.0,
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

		string resourceDirectory = "../resources" ;
		// Initialize mesh.

		shape.loadMesh(resourceDirectory + "/smoothsphere.obj");
		shape.resize();
		shape.init();

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
		prog->addAttribute("vertColor");
		prog->addUniform("diffcol");

		prog2 = std::make_shared<Program>();
		prog2->setVerbose(true);
		prog2->setShaderNames(resourceDirectory + "/shader_vertex.glsl", resourceDirectory + "/shader_fragment2.glsl");
		prog2->init();
		prog2->addUniform("P");
		prog2->addUniform("V");
		prog2->addUniform("M");
		prog2->addAttribute("vertPos");
		prog2->addAttribute("vertColor");
	}


	/****DRAW
	This is the most important function in your program - this is where you
	will actually issue the commands to draw any geometry you have set up to
	draw
	********/
	float RandomNumber(float Min, float Max)
	{
		return ((float(rand()) / float(RAND_MAX)) * (Max - Min)) + Min;
	}
	void render()
	{
		static int P1points = 0;
		static int P2points = 0;
		static float offset = 0.0f;
		float leftWallx = -1.4f;
		float rightWallx = 1.4f;
		float Wallz1 = -3.5f;
		float Wallz2 = -10.5f;
		float rightWall = 1.4f;
		double frametime = get_last_elapsed_time();
		static float move = 0.0f;
		static float move2 = 0.0f;
		static float position = 0.0f;
		static float position2 = 0.0f;
		vec3 initialBallPos = vec3(0.0f, -0.91, -7.0f);
		vec3 initialBallVel = vec3(RandomNumber(-0.002f, 0.01f), 0.0f, RandomNumber(0.01f, 0.015f));
		static vec3 ballPosition = vec3(0.0f, -0.91, -7.0f);
		static vec3 ballVelocity = vec3(RandomNumber(-0.002f, 0.01f), 0.0f, 0.01);
		// Get current frame buffer size.
		vec3 c;
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

		////animation with the model matrix:
		//static float w = 0.0;
		//w += 1.0 * frametime;//rotation angle
		//float trans = 0;// sin(t) * 2;
		//glm::mat4 RotateY = glm::rotate(glm::mat4(1.0f), w, glm::vec3(0.0f, 1.0f, 0.0f));
		//float angle = -3.1415926/2.0;

		//glm::mat4 RotateX = glm::rotate(glm::mat4(1.0f), angle, glm::vec3(1.0f, 0.0f, 0.0f));
		//glm::mat4 TransZ = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -3 + trans));
		//glm::mat4 S = glm::scale(glm::mat4(1.0f), glm::vec3(0.8f, 0.8f, 0.8f));

		//M =  TransZ * RotateY * RotateX * S;

		// Draw the box using GLSL.
		prog->bind();
		glBindVertexArray(VertexArrayID);

		//V = mycam.process(frametime);
		//send the matrices to the shaders

		// Board
		c.x = 0;
		c.y = 0;
		c.z = 0;
		mat4 trans = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -2.0f, -7.0f));
		mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(1.50, 1.0, 4.0f));
		M = trans * scale;
		glUniformMatrix4fv(prog->getUniform("P"), 1, GL_FALSE, &P[0][0]);
		glUniformMatrix4fv(prog->getUniform("V"), 1, GL_FALSE, &V[0][0]);
		glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, &M[0][0]);
		glUniform3fv(prog->getUniform("diffcol"), 1, value_ptr(c));
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, (void*)0);

		// Wall 1

		c.x = 1;
		c.y = 0;
		c.z = 1;
		mat4 transW = glm::translate(glm::mat4(1.0f), glm::vec3(-1.5f, -1.0f, -7.0f));
		mat4 scaleW = glm::scale(glm::mat4(1.0f), glm::vec3(0.1, 0.1, 3.5f));
		M = transW * scaleW;
		glUniformMatrix4fv(prog->getUniform("P"), 1, GL_FALSE, &P[0][0]);
		glUniformMatrix4fv(prog->getUniform("V"), 1, GL_FALSE, &V[0][0]);
		glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, &M[0][0]);
		glUniform3fv(prog->getUniform("diffcol"), 1, value_ptr(c));
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, (void*)0);

		// Wall 2
		c.x = 1;
		c.y = 0;
		c.z = 1;
		mat4 transW2 = glm::translate(glm::mat4(1.0f), glm::vec3(1.5f, -1.0f, -7.0f));
		mat4 scaleW2 = glm::scale(glm::mat4(1.0f), glm::vec3(0.1, 0.1, 3.5f));
		M = transW2 * scaleW2;
		glUniformMatrix4fv(prog->getUniform("P"), 1, GL_FALSE, &P[0][0]);
		glUniformMatrix4fv(prog->getUniform("V"), 1, GL_FALSE, &V[0][0]);
		glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, &M[0][0]);
		glUniform3fv(prog->getUniform("diffcol"), 1, value_ptr(c));
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, (void*)0);


		if (moveStatus == -1 && position>= -1.30f)
		{
			move = -1.2f * frametime;
			position += move;
		}
		if (moveStatusR == 1 && position <= 1.30f)
		{
			move = 1.2f * frametime;
			position += move;
		}
		/*if(moveStatus == 0)
			move = 0.0f;*/
		//cout << position << endl;
		// Player 1
		c.x = 1;
		c.y = 0;
		c.z = 0;
		mat4 translateP1 = glm::translate(glm::mat4(1.0f), glm::vec3(position, -1.0f, -3.5f));
		mat4 scaleP1 = glm::scale(glm::mat4(1.0f), glm::vec3(0.2, 0.1, 0.05));
		M = translateP1 * scaleP1; 
		glUniformMatrix4fv(prog->getUniform("P"), 1, GL_FALSE, &P[0][0]);
		glUniformMatrix4fv(prog->getUniform("V"), 1, GL_FALSE, &V[0][0]);
		glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, &M[0][0]);
		glUniform3fv(prog->getUniform("diffcol"), 1, value_ptr(c));
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, (void*)0);


		if (moveStatus2 == -1 && position2 >= -1.30f)
		{
			move2 = -1.5f * frametime;
			position2 += move2;
		}
		if (moveStatus2R == 1 && position2 <= 1.30f)
		{
			move2 = 1.5f * frametime;
			position2 += move2;
		}
		/*if (moveStatus2 == 0)
			move2 = 0.0f;*/
		// Player 2
		c.x = 0.1;
		c.y = 1;
		c.z = 0;
		mat4 translateP2 = glm::translate(glm::mat4(1.0f), glm::vec3(position2, -1.0f, -10.5f));
		mat4 scaleP2 = glm::scale(glm::mat4(1.0f), glm::vec3(0.2, 0.1, 0.05));
		M = translateP2 * scaleP2;
		glUniformMatrix4fv(prog->getUniform("P"), 1, GL_FALSE, &P[0][0]);
		glUniformMatrix4fv(prog->getUniform("V"), 1, GL_FALSE, &V[0][0]);
		glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, &M[0][0]);
		glUniform3fv(prog->getUniform("diffcol"), 1, value_ptr(c));
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, (void*)0);
		cout << ballVelocity.x << endl;
		// P1 Points

		if (P1points >= 1)
		{
			c.x = 1;
			c.y = 0;
			c.z = 0;
			mat4 pointp1 = glm::translate(glm::mat4(1.0f), glm::vec3(-2.0f, 1.0f, -3.0f));
			mat4 pointscale = glm::scale(glm::mat4(1.0f), glm::vec3(0.01f, 0.1f, 0.01f));
			M = pointp1 * pointscale;
			glUniformMatrix4fv(prog->getUniform("P"), 1, GL_FALSE, &P[0][0]);
			glUniformMatrix4fv(prog->getUniform("V"), 1, GL_FALSE, &V[0][0]);
			glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, &M[0][0]);
			glUniform3fv(prog->getUniform("diffcol"), 1, value_ptr(c));
			glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, (void*)0);
		}
		if (P1points >= 2)
		{
			c.x = 1;
			c.y = 0;
			c.z = 0;
			mat4 pointp1 = glm::translate(glm::mat4(1.0f), glm::vec3(-1.9f, 1.0f, -3.0f));
			mat4 pointscale = glm::scale(glm::mat4(1.0f), glm::vec3(0.01f, 0.1f, 0.01f));
			M = pointp1 * pointscale;
			glUniformMatrix4fv(prog->getUniform("P"), 1, GL_FALSE, &P[0][0]);
			glUniformMatrix4fv(prog->getUniform("V"), 1, GL_FALSE, &V[0][0]);
			glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, &M[0][0]);
			glUniform3fv(prog->getUniform("diffcol"), 1, value_ptr(c));
			glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, (void*)0);
		}
		if (P1points >= 3)
		{
			c.x = 1;
			c.y = 0;
			c.z = 0;
			mat4 pointp1 = glm::translate(glm::mat4(1.0f), glm::vec3(-1.8f, 1.0f, -3.0f));
			mat4 pointscale = glm::scale(glm::mat4(1.0f), glm::vec3(0.01f, 0.1f, 0.01f));
			M = pointp1 * pointscale;
			glUniformMatrix4fv(prog->getUniform("P"), 1, GL_FALSE, &P[0][0]);
			glUniformMatrix4fv(prog->getUniform("V"), 1, GL_FALSE, &V[0][0]);
			glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, &M[0][0]);
			glUniform3fv(prog->getUniform("diffcol"), 1, value_ptr(c));
			glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, (void*)0);
		}
		if (P1points >= 4)
		{
			c.x = 1;
			c.y = 0;
			c.z = 0;
			mat4 pointp1 = glm::translate(glm::mat4(1.0f), glm::vec3(-1.7f, 1.0f, -3.0f));
			mat4 pointscale = glm::scale(glm::mat4(1.0f), glm::vec3(0.01f, 0.1f, 0.01f));
			M = pointp1 * pointscale;
			glUniformMatrix4fv(prog->getUniform("P"), 1, GL_FALSE, &P[0][0]);
			glUniformMatrix4fv(prog->getUniform("V"), 1, GL_FALSE, &V[0][0]);
			glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, &M[0][0]);
			glUniform3fv(prog->getUniform("diffcol"), 1, value_ptr(c));
			glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, (void*)0);
		}
		if (P1points >= 5)
		{
			c.x = 1;
			c.y = 0;
			c.z = 0;
			mat4 pointp1 = glm::translate(glm::mat4(1.0f), glm::vec3(-1.6f, 1.0f, -3.0f));
			mat4 pointscale = glm::scale(glm::mat4(1.0f), glm::vec3(0.01f, 0.1f, 0.01f));
			M = pointp1 * pointscale;
			glUniformMatrix4fv(prog->getUniform("P"), 1, GL_FALSE, &P[0][0]);
			glUniformMatrix4fv(prog->getUniform("V"), 1, GL_FALSE, &V[0][0]);
			glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, &M[0][0]);
			glUniform3fv(prog->getUniform("diffcol"), 1, value_ptr(c));
			glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, (void*)0);
		}

		if (P1points >= 6)
		{
			c.x = 1;
			c.y = 0;
			c.z = 0;
			mat4 pointp1 = glm::translate(glm::mat4(1.0f), glm::vec3(-2.0f, 0.7f, -3.0f));
			mat4 pointscale = glm::scale(glm::mat4(1.0f), glm::vec3(0.01f, 0.1f, 0.01f));
			M = pointp1 * pointscale;
			glUniformMatrix4fv(prog->getUniform("P"), 1, GL_FALSE, &P[0][0]);
			glUniformMatrix4fv(prog->getUniform("V"), 1, GL_FALSE, &V[0][0]);
			glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, &M[0][0]);
			glUniform3fv(prog->getUniform("diffcol"), 1, value_ptr(c));
			glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, (void*)0);
		}
		if (P1points >= 7)
		{
			c.x = 1;
			c.y = 0;
			c.z = 0;
			mat4 pointp1 = glm::translate(glm::mat4(1.0f), glm::vec3(-1.9f, 0.7f, -3.0f));
			mat4 pointscale = glm::scale(glm::mat4(1.0f), glm::vec3(0.01f, 0.1f, 0.01f));
			M = pointp1 * pointscale;
			glUniformMatrix4fv(prog->getUniform("P"), 1, GL_FALSE, &P[0][0]);
			glUniformMatrix4fv(prog->getUniform("V"), 1, GL_FALSE, &V[0][0]);
			glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, &M[0][0]);
			glUniform3fv(prog->getUniform("diffcol"), 1, value_ptr(c));
			glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, (void*)0);
		}
		if (P1points >= 8)
		{
			c.x = 1;
			c.y = 0;
			c.z = 0;
			mat4 pointp1 = glm::translate(glm::mat4(1.0f), glm::vec3(-1.8f, 0.7f, -3.0f));
			mat4 pointscale = glm::scale(glm::mat4(1.0f), glm::vec3(0.01f, 0.1f, 0.01f));
			M = pointp1 * pointscale;
			glUniformMatrix4fv(prog->getUniform("P"), 1, GL_FALSE, &P[0][0]);
			glUniformMatrix4fv(prog->getUniform("V"), 1, GL_FALSE, &V[0][0]);
			glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, &M[0][0]);
			glUniform3fv(prog->getUniform("diffcol"), 1, value_ptr(c));
			glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, (void*)0);
		}
		if (P1points >= 9)
		{
			c.x = 1;
			c.y = 0;
			c.z = 0;
			mat4 pointp1 = glm::translate(glm::mat4(1.0f), glm::vec3(-1.7f, 0.7f, -3.0f));
			mat4 pointscale = glm::scale(glm::mat4(1.0f), glm::vec3(0.01f, 0.1f, 0.01f));
			M = pointp1 * pointscale;
			glUniformMatrix4fv(prog->getUniform("P"), 1, GL_FALSE, &P[0][0]);
			glUniformMatrix4fv(prog->getUniform("V"), 1, GL_FALSE, &V[0][0]);
			glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, &M[0][0]);
			glUniform3fv(prog->getUniform("diffcol"), 1, value_ptr(c));
			glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, (void*)0);
		}
		if (P1points >= 10)
		{
			c.x = 1;
			c.y = 0;
			c.z = 0;
			mat4 pointp1 = glm::translate(glm::mat4(1.0f), glm::vec3(-1.6f, 0.7f, -3.0f));
			mat4 pointscale = glm::scale(glm::mat4(1.0f), glm::vec3(0.01f, 0.1f, 0.01f));
			M = pointp1 * pointscale;
			glUniformMatrix4fv(prog->getUniform("P"), 1, GL_FALSE, &P[0][0]);
			glUniformMatrix4fv(prog->getUniform("V"), 1, GL_FALSE, &V[0][0]);
			glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, &M[0][0]);
			glUniform3fv(prog->getUniform("diffcol"), 1, value_ptr(c));
			glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, (void*)0);
		}


		// P2 Points
		if (P2points >= 1)
		{
			c.x = 0.1;
			c.y = 1;
			c.z = 0;
			mat4 pointp1 = glm::translate(glm::mat4(1.0f), glm::vec3(1.6f, 1.0f, -3.0f));
			mat4 pointscale = glm::scale(glm::mat4(1.0f), glm::vec3(0.01f, 0.1f, 0.01f));
			M = pointp1 * pointscale;
			glUniformMatrix4fv(prog->getUniform("P"), 1, GL_FALSE, &P[0][0]);
			glUniformMatrix4fv(prog->getUniform("V"), 1, GL_FALSE, &V[0][0]);
			glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, &M[0][0]);
			glUniform3fv(prog->getUniform("diffcol"), 1, value_ptr(c));
			glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, (void*)0);
		}
		if (P2points >= 2)
		{
			c.x = 0.1;
			c.y = 1;
			c.z = 0;
			mat4 pointp1 = glm::translate(glm::mat4(1.0f), glm::vec3(1.7f, 1.0f, -3.0f));
			mat4 pointscale = glm::scale(glm::mat4(1.0f), glm::vec3(0.01f, 0.1f, 0.01f));
			M = pointp1 * pointscale;
			glUniformMatrix4fv(prog->getUniform("P"), 1, GL_FALSE, &P[0][0]);
			glUniformMatrix4fv(prog->getUniform("V"), 1, GL_FALSE, &V[0][0]);
			glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, &M[0][0]);
			glUniform3fv(prog->getUniform("diffcol"), 1, value_ptr(c));
			glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, (void*)0);
		}
		if (P2points >= 3)
		{
			c.x = 0.1;
			c.y = 1;
			c.z = 0;
			mat4 pointp1 = glm::translate(glm::mat4(1.0f), glm::vec3(1.8f, 1.0f, -3.0f));
			mat4 pointscale = glm::scale(glm::mat4(1.0f), glm::vec3(0.01f, 0.1f, 0.01f));
			M = pointp1 * pointscale;
			glUniformMatrix4fv(prog->getUniform("P"), 1, GL_FALSE, &P[0][0]);
			glUniformMatrix4fv(prog->getUniform("V"), 1, GL_FALSE, &V[0][0]);
			glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, &M[0][0]);
			glUniform3fv(prog->getUniform("diffcol"), 1, value_ptr(c));
			glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, (void*)0);
		}
		if (P2points >= 4)
		{
			c.x = 0.1;
			c.y = 1;
			c.z = 0;
			mat4 pointp1 = glm::translate(glm::mat4(1.0f), glm::vec3(1.9f, 1.0f, -3.0f));
			mat4 pointscale = glm::scale(glm::mat4(1.0f), glm::vec3(0.01f, 0.1f, 0.01f));
			M = pointp1 * pointscale;
			glUniformMatrix4fv(prog->getUniform("P"), 1, GL_FALSE, &P[0][0]);
			glUniformMatrix4fv(prog->getUniform("V"), 1, GL_FALSE, &V[0][0]);
			glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, &M[0][0]);
			glUniform3fv(prog->getUniform("diffcol"), 1, value_ptr(c));
			glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, (void*)0);
		}
		if (P2points >= 5)
		{
			c.x = 0.1;
			c.y = 1;
			c.z = 0;
			mat4 pointp1 = glm::translate(glm::mat4(1.0f), glm::vec3(2.0f, 1.0f, -3.0f));
			mat4 pointscale = glm::scale(glm::mat4(1.0f), glm::vec3(0.01f, 0.1f, 0.01f));
			M = pointp1 * pointscale;
			glUniformMatrix4fv(prog->getUniform("P"), 1, GL_FALSE, &P[0][0]);
			glUniformMatrix4fv(prog->getUniform("V"), 1, GL_FALSE, &V[0][0]);
			glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, &M[0][0]);
			glUniform3fv(prog->getUniform("diffcol"), 1, value_ptr(c));
			glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, (void*)0);
		}

		if (P2points >= 6)
		{
			c.x = 0.1;
			c.y = 1;
			c.z = 0;
			mat4 pointp1 = glm::translate(glm::mat4(1.0f), glm::vec3(1.6f, 0.7f, -3.0f));
			mat4 pointscale = glm::scale(glm::mat4(1.0f), glm::vec3(0.01f, 0.1f, 0.01f));
			M = pointp1 * pointscale;
			glUniformMatrix4fv(prog->getUniform("P"), 1, GL_FALSE, &P[0][0]);
			glUniformMatrix4fv(prog->getUniform("V"), 1, GL_FALSE, &V[0][0]);
			glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, &M[0][0]);
			glUniform3fv(prog->getUniform("diffcol"), 1, value_ptr(c));
			glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, (void*)0);
		}
		if (P2points >= 7)
		{
			c.x = 0.1;
			c.y = 1;
			c.z = 0;
			mat4 pointp1 = glm::translate(glm::mat4(1.0f), glm::vec3(1.7f, 0.7f, -3.0f));
			mat4 pointscale = glm::scale(glm::mat4(1.0f), glm::vec3(0.01f, 0.1f, 0.01f));
			M = pointp1 * pointscale;
			glUniformMatrix4fv(prog->getUniform("P"), 1, GL_FALSE, &P[0][0]);
			glUniformMatrix4fv(prog->getUniform("V"), 1, GL_FALSE, &V[0][0]);
			glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, &M[0][0]);
			glUniform3fv(prog->getUniform("diffcol"), 1, value_ptr(c));
			glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, (void*)0);
		}
		if (P2points >= 8)
		{
			c.x = 0.1;
			c.y = 1;
			c.z = 0;
			mat4 pointp1 = glm::translate(glm::mat4(1.0f), glm::vec3(1.8f, 0.7f, -3.0f));
			mat4 pointscale = glm::scale(glm::mat4(1.0f), glm::vec3(0.01f, 0.1f, 0.01f));
			M = pointp1 * pointscale;
			glUniformMatrix4fv(prog->getUniform("P"), 1, GL_FALSE, &P[0][0]);
			glUniformMatrix4fv(prog->getUniform("V"), 1, GL_FALSE, &V[0][0]);
			glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, &M[0][0]);
			glUniform3fv(prog->getUniform("diffcol"), 1, value_ptr(c));
			glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, (void*)0);
		}
		if (P2points >= 9)
		{
			c.x = 0.1;
			c.y = 1;
			c.z = 0;
			mat4 pointp1 = glm::translate(glm::mat4(1.0f), glm::vec3(1.9f, 0.7f, -3.0f));
			mat4 pointscale = glm::scale(glm::mat4(1.0f), glm::vec3(0.01f, 0.1f, 0.01f));
			M = pointp1 * pointscale;
			glUniformMatrix4fv(prog->getUniform("P"), 1, GL_FALSE, &P[0][0]);
			glUniformMatrix4fv(prog->getUniform("V"), 1, GL_FALSE, &V[0][0]);
			glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, &M[0][0]);
			glUniform3fv(prog->getUniform("diffcol"), 1, value_ptr(c));
			glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, (void*)0);
		}
		if (P2points >= 10)
		{
			c.x = 0.1;
			c.y = 1;
			c.z = 0;
			mat4 pointp1 = glm::translate(glm::mat4(1.0f), glm::vec3(2.0f, 0.7f, -3.0f));
			mat4 pointscale = glm::scale(glm::mat4(1.0f), glm::vec3(0.01f, 0.1f, 0.01f));
			M = pointp1 * pointscale;
			glUniformMatrix4fv(prog->getUniform("P"), 1, GL_FALSE, &P[0][0]);
			glUniformMatrix4fv(prog->getUniform("V"), 1, GL_FALSE, &V[0][0]);
			glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, &M[0][0]);
			glUniform3fv(prog->getUniform("diffcol"), 1, value_ptr(c));
			glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, (void*)0);
		}
		

		prog->unbind();
		prog2->bind();
		static mat4 rotateBall = mat4(1.0f);
		static float r2;
		static float r3;
		if (P1points != 10 && P2points != 10)
		{
			float leftP1 = (-1 * 0.2) + position;
			float rightP1 = (1 * 0.2) + position;
			float leftP2 = (-1 * 0.2) + position2;
			float rightP2 = (1 * 0.2) + position2;
			if ((ballPosition.x >= leftP1 && ballPosition.x <= rightP1) && (ballPosition.z >= -3.6f && ballPosition.z <= -3.55f))
			{
				
				ballVelocity = (abs(ballVelocity));
				r2 = RandomNumber(-0.004f, 0.004f);
				ballVelocity *= -1;
				if (ballVelocity.x <= -0.1 && ballVelocity.x >= 0.1 && r2 >= 0.0f)
					r2 = 0;
				ballVelocity.x = ballVelocity.x + r2;
				if (moveStatusR == 1 || moveStatus == -1)
					ballVelocity *= 1.25f;

			}
			if ((ballPosition.x >= leftP2 && ballPosition.x <= rightP2) && (ballPosition.z <= -10.4f && ballPosition.z >= -10.45))
			{
				ballVelocity = (abs(ballVelocity));
				r3 = RandomNumber(-0.004f, 0.004f);
				if (ballVelocity.x <= -0.1 && ballVelocity.x >= 0.1 && r3 >= 0.0f)
					r3 = 0;
				ballVelocity.x = ballVelocity.x + r3;
				if (moveStatus2R == 1 || moveStatus2 == -1)
					ballVelocity *= 1.25f;

			}
			if ((ballPosition.x <= leftWallx) && (ballPosition.z <= Wallz1 && ballPosition.z >= Wallz2))
			{
				ballVelocity.x = (abs(ballVelocity.x));
			}
			if ((ballPosition.x >= rightWallx) && (ballPosition.z <= Wallz1 && ballPosition.z >= Wallz2))
			{
				
				ballVelocity.x = (abs(ballVelocity.x));
				ballVelocity.x *= -1;
			}
			ballPosition += ballVelocity;

			if (ballPosition.z >= -2.0f || ballPosition.z <= -11.0f)
			{
				if (ballPosition.z >= -2.0f)
				{
					ballVelocity = -initialBallVel;
					P2points += 1;
				}
				if (ballPosition.z <= -11.0f)
				{
					ballVelocity = initialBallVel;
					P1points += 1;
				}
				ballPosition = initialBallPos;



			}
		}

		mat4 translateBall = glm::translate(glm::mat4(1.0f), ballPosition);
		mat4 scaleBall = glm::scale(glm::mat4(1.0f), glm::vec3(0.07, 0.07, 0.07));
		M = translateBall*rotateBall* scaleBall;
		glUniformMatrix4fv(prog->getUniform("P"), 1, GL_FALSE, &P[0][0]);
		glUniformMatrix4fv(prog->getUniform("V"), 1, GL_FALSE, &V[0][0]);
		glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, &M[0][0]);
		shape.draw(prog2);
		prog2->unbind();
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
