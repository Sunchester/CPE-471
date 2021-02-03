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
float armRotation = 0;
float armRotationDown = 0;
float armRotationRight = 0;
float armRotationRightDown = 0;
class Application : public EventCallbacks
{

public:

	WindowManager * windowManager = nullptr;

	// Our shader program
	std::shared_ptr<Program> prog;

	// Contains vertex information for OpenGL
	GLuint VertexArrayID, triVAO;

	// Data necessary to give our box to OpenGL
	GLuint VertexBufferID, VertexColorIDBox, IndexBufferIDBox, triVBO, triColor;

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
		if (key == GLFW_KEY_M && action == GLFW_PRESS)
		{
			armRotation = 1;
		}

		if (key == GLFW_KEY_M && action == GLFW_RELEASE)
		{
			armRotation = 0;
		}

		if (key == GLFW_KEY_N && action == GLFW_PRESS)
		{
			armRotationDown = 1;
		}

		if (key == GLFW_KEY_N && action == GLFW_RELEASE)
		{
			armRotationDown = 0;
		}
		if (key == GLFW_KEY_K && action == GLFW_PRESS)
		{
			armRotationRight = 1;
		}

		if (key == GLFW_KEY_K && action == GLFW_RELEASE)
		{
			armRotationRight = 0;
		}

		if (key == GLFW_KEY_J && action == GLFW_PRESS)
		{
			armRotationRightDown = 1;
		}

		if (key == GLFW_KEY_J && action == GLFW_RELEASE)
		{
			armRotationRightDown = 0;
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

		//generate the VAO
		glGenVertexArrays(1, &triVAO);
		glBindVertexArray(triVAO);

		//generate vertex buffer to hand off to OGL
		glGenBuffers(1, &triVBO);
		//set the current state to focus on our vertex buffer
		glBindBuffer(GL_ARRAY_BUFFER, triVBO);

		static const GLfloat tri_v[] =
		{
			-0.5f, -0.5f, 0.0f,
			0.5f, -0.5f, 0.0f,
			0.0f, 0.7f, 0.0f,
			-0.5f, -0.5f, 0.0f,
			0.5f, -0.5f, 0.0f,
			0.0f, 0.7f, 0.0f,
		};
		glBufferData(GL_ARRAY_BUFFER, sizeof(tri_v), tri_v, GL_DYNAMIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		glGenBuffers(1, &triColor);
		//set the current state to focus on our vertex buffer
		glBindBuffer(GL_ARRAY_BUFFER, triColor);
		static const GLfloat colors[] =
		{
			1.0f, 0.0f, 0.0f,
			1.0f, 0.0f, 0.0f,
			1.0f, 1.0f, 1.0f,
			1.0f, 0.5490196078431373f, 0.0f,
			1.0f, 0.5490196078431373f, 0.0f,
			1.0f, 1.0f, 1.0f,
		}; 

		glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_DYNAMIC_DRAW);

		//we need to set up the vertex array
		glEnableVertexAttribArray(1);
		//key function to get up how many elements to pull out at a time (3)
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
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
		prog->addUniform("black");
		
	}


	/****DRAW
	This is the most important function in your program - this is where you
	will actually issue the commands to draw any geometry you have set up to
	draw
	********/
	void render()
	{
		static float upperL = 0.0;
		static float upperR = 0.0;
		static float LeftArm = 0.0;
		static float RightArm = 0.0;
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
		w += 1.0 * frametime;//rotation angle
		float trans = 0;// sin(t) * 2;
		glm::mat4 RotateY = glm::rotate(glm::mat4(1.0f), w, glm::vec3(0.0f, 1.0f, 0.0f));
		float angle = -3.1415926/2.0;

		/*glm::mat4 RotateX = glm::rotate(glm::mat4(1.0f), angle, glm::vec3(1.0f, 0.0f, 0.0f));
		glm::mat4 TransZ = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -3 + trans));
		glm::mat4 S = glm::scale(glm::mat4(1.0f), glm::vec3(0.8f, 0.8f, 0.8f));*/

		/*M =  TransZ * RotateY * RotateX * S;*/

		// Draw the box using GLSL.
		prog->bind();

		vec3 b(0, 0, 0);
		V = mycam.process(frametime);
		glBindVertexArray(VertexArrayID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndexBufferIDBox);

		mat4 rotAll = glm::rotate(mat4(1.0f), 0.349066f, vec3(0.0, 1.0, 0.0));
		// Middle Body of Snowman
		mat4 transl = glm::translate(mat4(1.0f), vec3(0.0f, 0.0, -3.0f));
		mat4 scale = glm::scale(mat4(1.0f), vec3(0.20f, 0.20f, 0.20f));
		M = transl * rotAll * scale;
		
		//send the matrices to the shaders
		glUniformMatrix4fv(prog->getUniform("P"), 1, GL_FALSE, &P[0][0]);
		glUniformMatrix4fv(prog->getUniform("V"), 1, GL_FALSE, &V[0][0]);
		glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, &M[0][0]);
		glUniform3fv(prog->getUniform("black"), 1, value_ptr(b));
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, (void*)0);
		/*shape.draw(prog);	*/	

		// Head of Snowman
		mat4 translate1 = glm::translate(mat4(1.0f), vec3(0.0f, 0.355f, -3.0f));
		mat4 scale1 = glm::scale(mat4(1.0f), vec3(0.15f, 0.15f, 0.15f));
		M =  translate1* rotAll  * scale1;
		glUniformMatrix4fv(prog->getUniform("P"), 1, GL_FALSE, &P[0][0]);
		glUniformMatrix4fv(prog->getUniform("V"), 1, GL_FALSE, &V[0][0]);
		glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, &M[0][0]);
		glUniform3fv(prog->getUniform("black"), 1, value_ptr(b));
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, (void*)0);


		// Bottom Body of Snowman
		mat4 translate2 = glm::translate(mat4(1.0f), vec3(0.0f, -0.45f, -3.0f));
		mat4 scale2 = glm::scale(mat4(1.0f), vec3(0.25f, 0.25f, 0.25f));
		M = translate2 * rotAll * scale2;
		glUniformMatrix4fv(prog->getUniform("P"), 1, GL_FALSE, &P[0][0]);
		glUniformMatrix4fv(prog->getUniform("V"), 1, GL_FALSE, &V[0][0]);
		glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, &M[0][0]);
		glUniform3fv(prog->getUniform("black"), 1, value_ptr(b));
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, (void*)0);

		// Button 1
		b.x = -1;
		b.y = -1;
		b.z= -1;
		mat4 translateBut = glm::translate(mat4(1.0f), vec3(0.0, 0.0, 0.25));
		mat4 translate3 = glm::translate(mat4(1.0f), vec3(0.0f, -0.60f, -3.0f));
		mat4 scale3 = glm::scale(mat4(1.0f), vec3(0.025f, 0.025f, 0.025f));
		M = translate3 * rotAll * translateBut * scale3;
		glUniformMatrix4fv(prog->getUniform("P"), 1, GL_FALSE, &P[0][0]);
		glUniformMatrix4fv(prog->getUniform("V"), 1, GL_FALSE, &V[0][0]);
		glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, &M[0][0]);
		glUniform3fv(prog->getUniform("black"), 1, value_ptr(b));
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, (void*)0);

		// Button 2
		b.x = -1;
		b.y = -1;
		b.z = -1;
		mat4 translate4 = glm::translate(mat4(1.0f), vec3(0.0f, -0.45f, -3.0f));
		M = translate4 * rotAll * translateBut * scale3;
		glUniformMatrix4fv(prog->getUniform("P"), 1, GL_FALSE, &P[0][0]);
		glUniformMatrix4fv(prog->getUniform("V"), 1, GL_FALSE, &V[0][0]);
		glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, &M[0][0]);
		glUniform3fv(prog->getUniform("black"), 1, value_ptr(b));
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, (void*)0);

		// Button 3
		b.x = -1;
		b.y = -1;
		b.z = -1;
		mat4 translate5 = glm::translate(mat4(1.0f), vec3(0.0f, -0.30f, -3.0f));
		M = translate5 * rotAll * translateBut * scale3;
		glUniformMatrix4fv(prog->getUniform("P"), 1, GL_FALSE, &P[0][0]);
		glUniformMatrix4fv(prog->getUniform("V"), 1, GL_FALSE, &V[0][0]);
		glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, &M[0][0]);
		glUniform3fv(prog->getUniform("black"), 1, value_ptr(b));
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, (void*)0);


		// Button 4
		b.x = -1;
		b.y = -1;
		b.z = -1;
		translateBut = glm::translate(mat4(1.0f), vec3(0.0, 0.0, 0.20));
		mat4 translate6 = glm::translate(mat4(1.0f), vec3(0.0f, -0.10f, -3.0f));
		M = translate6 * rotAll * translateBut * scale3;
		glUniformMatrix4fv(prog->getUniform("P"), 1, GL_FALSE, &P[0][0]);
		glUniformMatrix4fv(prog->getUniform("V"), 1, GL_FALSE, &V[0][0]);
		glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, &M[0][0]);
		glUniform3fv(prog->getUniform("black"), 1, value_ptr(b));
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, (void*)0);

		// Button 5
		b.x = -1;
		b.y = -1;
		b.z = -1;
		mat4 translate7 = glm::translate(mat4(1.0f), vec3(0.0f, 0.05f, -3.0f));
		M = translate7 * rotAll * translateBut * scale3;
		glUniformMatrix4fv(prog->getUniform("P"), 1, GL_FALSE, &P[0][0]);
		glUniformMatrix4fv(prog->getUniform("V"), 1, GL_FALSE, &V[0][0]);
		glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, &M[0][0]);
		glUniform3fv(prog->getUniform("black"), 1, value_ptr(b));
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, (void*)0);

		// Eye 1 
		b.x =  1;
		b.y = 0;
		b.z = 0;
		translateBut = glm::translate(mat4(1.0f), vec3(0.0, 0.0, 0.17));
		mat4 translate8 = glm::translate(mat4(1.0f), vec3(-0.06f, 0.40f, -3.0f));
		mat4 scale4 = glm::scale(mat4(1.0f), vec3(0.015f, 0.015f, 0.015f));
		M = translate8 * rotAll * translateBut * scale4;
		glUniformMatrix4fv(prog->getUniform("P"), 1, GL_FALSE, &P[0][0]);
		glUniformMatrix4fv(prog->getUniform("V"), 1, GL_FALSE, &V[0][0]);
		glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, &M[0][0]);
		glUniform3fv(prog->getUniform("black"), 1, value_ptr(b));
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, (void*)0);


		// Eye 2
		b.x = 1;
		b.y = 0;
		b.z = 0;

		translateBut = glm::translate(mat4(1.0f), vec3(0.12, 0.0, 0.17));
		mat4 translate9 = glm::translate(mat4(1.0f), vec3(-0.06f, 0.40f, -3.0f));
		M = translate9 * rotAll * translateBut * scale4;
		glUniformMatrix4fv(prog->getUniform("P"), 1, GL_FALSE, &P[0][0]);
		glUniformMatrix4fv(prog->getUniform("V"), 1, GL_FALSE, &V[0][0]);
		glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, &M[0][0]);
		glUniform3fv(prog->getUniform("black"), 1, value_ptr(b));
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, (void*)0);


		// Eye Brow 1
		b.x = 0.50196078431;
		b.y = 0;
		b.z = 0;

		mat4 browS = glm::scale(mat4(1.0f), vec3(0.045, 0.015, 0.01));
		mat4 browT = glm::translate(mat4(1.0f), vec3(-0.01, 0.48, -2.824f));
		mat4 browR = glm::rotate(mat4(1.0f), -0.785398f, vec3(0.0, 0.0, 1.0));
		M = browT*rotAll*browR*browS;
		glUniformMatrix4fv(prog->getUniform("P"), 1, GL_FALSE, &P[0][0]);
		glUniformMatrix4fv(prog->getUniform("V"), 1, GL_FALSE, &V[0][0]);
		glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, &M[0][0]);
		glUniform3fv(prog->getUniform("black"), 1, value_ptr(b));
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, (void*)0);



		// Eye Brow 2
		b.x = 0.50196078431;
		b.y = 0;
		b.z = 0;

		browT = glm::translate(mat4(1.0f), vec3(0.13, 0.48, -2.88f));
		browR = glm::rotate(mat4(1.0f), 0.785398f, vec3(0.0, 0.0, 1.0));
		M = browT * rotAll * browR * browS;
		glUniformMatrix4fv(prog->getUniform("P"), 1, GL_FALSE, &P[0][0]);
		glUniformMatrix4fv(prog->getUniform("V"), 1, GL_FALSE, &V[0][0]);
		glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, &M[0][0]);
		glUniform3fv(prog->getUniform("black"), 1, value_ptr(b));
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, (void*)0);


		// Mouth 
		b.x = -1;
		b.y = -1;
		b.z = -1;
		translateBut = glm::translate(mat4(1.0f), vec3(0.01, 0.0, 0.15));
		mat4 translateM = glm::translate(mat4(1.0f), vec3(0.0f, 0.25f, -3.0f));
		mat4 scaleM = glm::scale(mat4(1.0f), vec3(0.015f, 0.015f, 0.015f));
		mat4 rotateM = glm::rotate(mat4(1.0f), 0.785398f, vec3(0.0, 0.0, 1.0));
		M = translateM * rotAll * translateBut * rotateM* scaleM;
		glUniformMatrix4fv(prog->getUniform("P"), 1, GL_FALSE, &P[0][0]);
		glUniformMatrix4fv(prog->getUniform("V"), 1, GL_FALSE, &V[0][0]);
		glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, &M[0][0]);
		glUniform3fv(prog->getUniform("black"), 1, value_ptr(b));
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, (void*)0);

		// Mouth 1
		b.x = -1;
		b.y = -1;
		b.z = -1;
		translateBut = glm::translate(mat4(1.0f), vec3(0.06f, 0.005, 0.15));
		mat4 translateM1 = glm::translate(mat4(1.0f), vec3(0.0f, 0.25f, -3.0f));
		mat4 scaleM1 = glm::scale(mat4(1.0f), vec3(0.015f, 0.015f, 0.015f));
		mat4 rotateM1 = glm::rotate(mat4(1.0f), 0.785398f, vec3(0.0, 0.0, 1.0));
		M = translateM1 * rotAll * translateBut * rotateM1 * scaleM1;
		glUniformMatrix4fv(prog->getUniform("P"), 1, GL_FALSE, &P[0][0]);
		glUniformMatrix4fv(prog->getUniform("V"), 1, GL_FALSE, &V[0][0]);
		glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, &M[0][0]);
		glUniform3fv(prog->getUniform("black"), 1, value_ptr(b));
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, (void*)0);
	
		// Mouth 2
		b.x = -1;
		b.y = -1;
		b.z = -1;
		translateBut = glm::translate(mat4(1.0f), vec3(0.11f, 0.025, 0.15));
		mat4 translateM2 = glm::translate(mat4(1.0f), vec3(0.0f, 0.25f, -3.0f));
		mat4 scaleM2 = glm::scale(mat4(1.0f), vec3(0.015f, 0.015f, 0.015f));
		mat4 rotateM2 = glm::rotate(mat4(1.0f), 0.785398f, vec3(0.0, 0.0, 1.0));
		M = translateM2 * rotAll * translateBut * rotateM2 * scaleM2;
		glUniformMatrix4fv(prog->getUniform("P"), 1, GL_FALSE, &P[0][0]);
		glUniformMatrix4fv(prog->getUniform("V"), 1, GL_FALSE, &V[0][0]);
		glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, &M[0][0]);
		glUniform3fv(prog->getUniform("black"), 1, value_ptr(b));
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, (void*)0);

		// Mouth 3
		b.x = -1;
		b.y = -1;
		b.z = -1;
		translateBut = glm::translate(mat4(1.0f), vec3(-0.04f, 0.005, 0.15));
		mat4 translateM3 = glm::translate(mat4(1.0f), vec3(0.0f, 0.25f, -3.0f));
		mat4 scaleM3 = glm::scale(mat4(1.0f), vec3(0.015f, 0.015f, 0.015f));
		mat4 rotateM3 = glm::rotate(mat4(1.0f), 0.785398f, vec3(0.0, 0.0, 1.0));
		M = translateM3 * rotAll * translateBut * rotateM1 * scaleM3;
		glUniformMatrix4fv(prog->getUniform("P"), 1, GL_FALSE, &P[0][0]);
		glUniformMatrix4fv(prog->getUniform("V"), 1, GL_FALSE, &V[0][0]);
		glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, &M[0][0]);
		glUniform3fv(prog->getUniform("black"), 1, value_ptr(b));
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, (void*)0);

		// Mouth 4
		b.x = -1;
		b.y = -1;
		b.z = -1;
		translateBut = glm::translate(mat4(1.0f), vec3(-0.08f, 0.025, 0.15));
		mat4 translateM4 = glm::translate(mat4(1.0f), vec3(0.0f, 0.25f, -3.0f));
		mat4 scaleM4 = glm::scale(mat4(1.0f), vec3(0.015f, 0.015f, 0.015f));
		mat4 rotateM4 = glm::rotate(mat4(1.0f), 0.785398f, vec3(0.0, 0.0, 1.0));
		M = translateM4 * rotAll * translateBut * rotateM4 * scaleM4;
		glUniformMatrix4fv(prog->getUniform("P"), 1, GL_FALSE, &P[0][0]);
		glUniformMatrix4fv(prog->getUniform("V"), 1, GL_FALSE, &V[0][0]);
		glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, &M[0][0]);
		glUniform3fv(prog->getUniform("black"), 1, value_ptr(b));
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, (void*)0);

		if (armRotation == 1 && LeftArm <= 1.55f)
		{
			LeftArm += 0.01;
			if(upperL <= 1.85)
				upperL += 0.011;
			std::cout << upperL << endl;
		}

		if (armRotationDown == 1 && LeftArm >= -0.7f)
		{
			LeftArm -= 0.01;
			if (upperL >= -1.85)
				upperL -= 0.011;
		}


		if (armRotationRight == 1 && RightArm <= 1.55f)
		{
			RightArm += 0.01;
			if (upperR <= 1.85)
				upperR += 0.011;
		}

		if (armRotationRightDown == 1 && RightArm >= -0.7f)
		{
			RightArm -= 0.01;
			if (upperR >= -1.85)
				upperR -= 0.011;
		}
		

		// Left Lower Arm 
		b.x = 0.5450980392156863;
		b.y = 0.2705882352941176;
		b.z = 0.0745098039215686;
		
		mat4 armLoT = glm::translate(mat4(1.0f), vec3(-0.205f, 0.1f, -3.0f));
		mat4 armLoTO = glm::translate(mat4(1.0f), vec3(0.0f, -0.15f, 0.0f));
		mat4 armLoR = glm::rotate(mat4(1.0f), -0.785398f - LeftArm, vec3(0.0, 0.0, 1.0));
		mat4 armLoS = glm::scale(mat4(1.0f),vec3(0.02, 0.15, 0.02));
		M = armLoT * rotAll * armLoR * armLoTO * armLoS;
		mat4 LoM = armLoT* armLoR * armLoTO;
		glUniformMatrix4fv(prog->getUniform("P"), 1, GL_FALSE, &P[0][0]);
		glUniformMatrix4fv(prog->getUniform("V"), 1, GL_FALSE, &V[0][0]);
		glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, &M[0][0]);
		glUniform3fv(prog->getUniform("black"), 1, value_ptr(b));
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, (void*)0);

		// Left Upper Arm
		b.x = 0.5450980392156863;
		b.y = 0.2705882352941176;
		b.z = 0.0745098039215686;

		mat4 armLuT = glm::translate(mat4(1.0f), vec3(-0.001f, -0.15f, 0.105f));
		mat4 armLuTO = glm::translate(mat4(1.0f), vec3(0.0f, 0.15f, 0.0f));
		mat4 armLuR = glm::rotate(mat4(1.0f), 1.309f + upperL, vec3(0.0, 0.0, 1.0));
		mat4 armLuS = glm::scale(mat4(1.0f), vec3(0.02, 0.15, 0.02));
		M = LoM * armLuT * rotAll* armLuR * armLuTO * armLuS;
		glUniformMatrix4fv(prog->getUniform("P"), 1, GL_FALSE, &P[0][0]);
		glUniformMatrix4fv(prog->getUniform("V"), 1, GL_FALSE, &V[0][0]);
		glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, &M[0][0]);
		glUniform3fv(prog->getUniform("black"), 1, value_ptr(b));
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, (void*)0);

		// Right Lower Arm
		b.x = 0.5450980392156863;
		b.y = 0.2705882352941176;
		b.z = 0.0745098039215686;

		mat4 armRoT = glm::translate(mat4(1.0f), vec3(0.205f, 0.1f, -3.0f));
		mat4 armRoTO = glm::translate(mat4(1.0f), vec3(0.0f, -0.15f, 0.0f));
		mat4 armRoR = glm::rotate(mat4(1.0f), 0.785398f + RightArm, vec3(0.0, 0.0, 1.0));
		mat4 armRoS = glm::scale(mat4(1.0f), vec3(0.02, 0.15, 0.02));
		M =  armRoT * rotAll * armRoR * armRoTO * armRoS;
		mat4 RoM = armRoT * armRoR * armRoTO;
		glUniformMatrix4fv(prog->getUniform("P"), 1, GL_FALSE, &P[0][0]);
		glUniformMatrix4fv(prog->getUniform("V"), 1, GL_FALSE, &V[0][0]);
		glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, &M[0][0]);
		glUniform3fv(prog->getUniform("black"), 1, value_ptr(b));
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, (void*)0);


		// Right Upper Arm
		b.x = 0.5450980392156863;
		b.y = 0.2705882352941176;
		b.z = 0.0745098039215686;

		mat4 armRuT = glm::translate(mat4(1.0f), vec3(0.001f, -0.15f, -0.075f));
		mat4 armRuTO = glm::translate(mat4(1.0f), vec3(0.0f, 0.15f, 0.0f));
		mat4 armRuR = glm::rotate(mat4(1.0f), -1.309f - upperR, vec3(0.0, 0.0, 1.0));
		mat4 armRuS = glm::scale(mat4(1.0f), vec3(0.02, 0.15, 0.02));
		M = RoM * armRuT * rotAll * armRuR * armRuTO * armRuS;

		glUniformMatrix4fv(prog->getUniform("P"), 1, GL_FALSE, &P[0][0]);
		glUniformMatrix4fv(prog->getUniform("V"), 1, GL_FALSE, &V[0][0]);
		glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, &M[0][0]);
		glUniform3fv(prog->getUniform("black"), 1, value_ptr(b));
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, (void*)0);

		glBindVertexArray(0);

		// Hat
		glBindVertexArray(triVAO);
		b.x = 0;
		b.y = 0;
		b.z = 0;
		mat4 hatS = glm::scale(mat4(1.0f), vec3(0.25f, 0.45f, 0.9f));
		mat4 hatT = glm::translate(mat4(1.0f), vec3(0.0, 0.65f, -3.0f));
		M = hatT*rotAll* hatS;
		glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, &M[0][0]);
		glUniform3fv(prog->getUniform("black"), 1, value_ptr(b));
		glDrawArrays(GL_TRIANGLES, 0, 3);


		// Nose
		b.x = 0;
		b.y = 0;
		b.z = 0;
		mat4 noseS = glm::scale(mat4(1.0f), vec3(0.1f, 0.3f, 0.9f));
		mat4 noseT = glm::translate(mat4(1.0f), vec3(0.08, 0.35f, -2.8f));
		mat4 noseR = glm::rotate(mat4(1.0f), 1.5708f, vec3(0.0, 1.0, 0.0));
		mat4 noseR2 = glm::rotate(mat4(1.0f), 1.5708f, vec3(1.0, 0.0, 0.0));
		M = noseT * rotAll* noseR2* noseR * noseS;
		glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, &M[0][0]);
		glUniform3fv(prog->getUniform("black"), 1, value_ptr(b));
		glDrawArrays(GL_TRIANGLES, 3, 6);
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
