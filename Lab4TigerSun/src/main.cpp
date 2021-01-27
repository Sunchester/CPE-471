/*
ZJ Wood CPE 471 Lab 3 base code
*/

#include <iostream>
#include <glad/glad.h>

#include "GLSL.h"
#include "Program.h"
#include "MatrixStack.h"

#include "WindowManager.h"

// value_ptr for glm
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
class matrix
{
	public:
		float M[4][4];
		void createIdentityMat()
		{	
			for (int i = 0; i < 4; i++)
			{
				for (int j = 0; j < 4; j++)
				{
					M[i][j] = 0.0f;
				}
			}
			int i2 = 0;

			for (int j2 = 0; j2 < 4; j2++)
			{
				M[i2][j2] = 1.0f;
				i2 += 1;
			}
		}
		void operator=(const matrix& rhs)
		{
			for (int i = 0; i < 4; i++)
			{
				for (int j = 0; j < 4; j++)
				{
					M[i][j] = rhs.M[i][j];
				}
			}
		}
		void transposeMat()
		{
			float copy[4][4];
			for (int i = 0; i < 4; i++)
			{
				for (int j = 0; j < 4; j++)
				{
					copy[i][j] = M[i][j];
				}
			}
			for (int mi = 0; mi < 4; mi++)
			{
				for (int mj = 0; mj < 4; mj++)
				{
					M[mi][mj] = copy[mj][mi];
				}
			}
		}
		void createTranslateMat(float x, float y, float z)
		{
			M[3][0] = x;
			M[3][1] = y;
			M[3][2] = z;
		}
		void createScaleMat(float x, float y, float z)
		{
			M[0][0] = x;
			M[1][1] = y;
			M[2][2] = z;
		}
		void createRotationMatX(float angle)
		{
			M[1][1] = cos(angle);
			M[1][2] = -1 * sin(angle);
			M[2][1] = sin(angle);
			M[2][2] = cos(angle);
			transposeMat();
			
		}
		void createRotationMatY(float angle)
		{
			M[0][0] = cos(angle);
			M[0][2] = sin(angle);
			M[2][0] = -1 * sin(angle);
			M[2][2] = cos(angle);
			transposeMat();

		}
		void createRotationMatZ(float angle)
		{
			M[0][0] = cos(angle);
			M[0][1] = -1 * sin(angle);
			M[1][0] = sin(angle);
			M[1][1] = cos(angle);
			transposeMat();


		}
};
matrix operator*(const matrix lhs, const matrix rhs)
{
	matrix result;
	result.createIdentityMat();
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			float first = rhs.M[i][0] * lhs.M[0][j];
			float second = rhs.M[i][1] * lhs.M[1][j];
			float third = rhs.M[i][2] * lhs.M[2][j];
			float fourth = rhs.M[i][3] * lhs.M[3][j];
			result.M[i][j] = first + second + third + fourth;
		}
	}
	return result;
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
	int w, a, s, d;
	camera()
	{
		w = a = s = d = 0;
		pos = rot = glm::vec3(0, 0, -1);
	}
	glm::mat4 process()
	{
		double ftime = get_last_elapsed_time();
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
float isShake = 0;
class Application : public EventCallbacks
{

public:

	WindowManager * windowManager = nullptr;

	// Our shader program
	Program prog;

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
		if (key == GLFW_KEY_X && action == GLFW_PRESS)
		{
			isShake = 1;
		}

		if (key == GLFW_KEY_C && action == GLFW_PRESS)
		{
			isShake = 0;
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
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*) 0);

		//color
		GLfloat cube_colors[] = {
			// front colors
			1.0, 0.0, 0.5,
			1.0, 0.0, 0.5,
			1.0, 0.0, 0.5,
			1.0, 0.0, 0.5,
			// back colors
			0.5, 0.5, 0.0,
			0.5, 0.5, 0.0,
			0.5, 0.5, 0.0,
			0.5, 0.5, 0.0,
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
		
		prog.setVerbose(true);
		prog.setShaderNames(resourceDirectory + "/shader_vertex.glsl", resourceDirectory + "/shader_fragment.glsl");
		if (!prog.init())
			{
			std::cerr << "One or more shaders failed to compile... exiting!" << std::endl;
			exit(1); //[SHADERBUG] ? breakpoint here!! if program hits that codeline, check the command line window, it will contain the line and place of the error in the shader file
			}
		prog.addUniform("P");
		prog.addUniform("V");
		prog.addUniform("M");
		prog.addAttribute("vertPos");
		prog.addAttribute("vertColor");
	}


	/****DRAW
	This is the most important function in your program - this is where you
	will actually issue the commands to draw any geometry you have set up to
	draw
	********/
	void render()
	{
		static float LeftArm = 0.0;
		static float Shake = 0.0;
		// Get current frame buffer size.
		int width, height;
		glfwGetFramebufferSize(windowManager->getHandle(), &width, &height);
		float aspect = width/(float)height;
		glViewport(0, 0, width, height);
		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		// Clear framebuffer.
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Create the matrix stacks - please leave these alone for now
		
		/*glm::mat4 V, M,  P;*/ //View, Model and Perspective matrix
		glm::mat4 V, P;
		V = glm::mat4(1);
		matrix Model; 
		Model.createIdentityMat();
		/*M = glm::mat4(1);*/
		P = glm::perspective((float)(3.14159 / 4.), (float)((float)width/ (float)height), 0.1f, 1000.0f); //so much type casting... GLM metods are quite funny ones
		
		//animation with the model matrix:
		static float w = 0.0;
		w += 0.01;//rotation angle
		static float t = 0;
		t += 0.01;
		float trans = 0;// sin(t) * 2;


		// Torso 

		matrix T;
		T.createIdentityMat();
		//glm::mat4 T = glm::mat4(1.0f);

		matrix RotateX;
		RotateX.createIdentityMat();
		RotateX.createRotationMatX(w);
		//glm::mat4 RotateX = glm::rotate(glm::mat4(1.0f), w, glm::vec3(-1.0f, 1.0f, 0.0f));

		matrix TransZ;
		TransZ.createIdentityMat();
		TransZ.createTranslateMat(0.0f, 0.0f, -3 + trans);
		//glm::mat4 TransZ = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -3 + trans));

		matrix TransX;
		TransX.createIdentityMat();
		TransX.createTranslateMat(0.4f, 0.0f, 0.0f);

		matrix S;
		S.createIdentityMat();
		S.createScaleMat(0.15f, 0.15f, 0.1f);

		
		// Draw the box using GLSL.
		prog.bind();

		V = mycam.process();
		//send the matrices to the shaders
		glUniformMatrix4fv(prog.getUniform("P"), 1, GL_FALSE, &P[0][0]);
		glUniformMatrix4fv(prog.getUniform("V"), 1, GL_FALSE, &V[0][0]);
		glUniformMatrix4fv(prog.getUniform("M"), 1, GL_FALSE, &Model.M[0][0]);

		glBindVertexArray(VertexArrayID);
		//actually draw from vertex 0, 3 vertices
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndexBufferIDBox);
		
		TransX.createTranslateMat(0, 0, 0);
		Model = TransX  * S;
		glUniformMatrix4fv(prog.getUniform("M"), 1, GL_FALSE, &Model.M[0][0]);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, (void*)0);
				

		// Head
		matrix head;
		head.createIdentityMat();
		matrix headT, headS, headR;
		headT.createIdentityMat();
		headS.createIdentityMat();
		headR.createIdentityMat();

		headT.createTranslateMat(0.0f, 0.225f, 0.0f);
		headR.createRotationMatY(0.785398);
		headS.createScaleMat(0.07f, 0.07f, 0.07f);
		head = headT * headR *headS;
		glUniformMatrix4fv(prog.getUniform("M"), 1, GL_FALSE, &head.M[0][0]);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, (void*)0);


		

		// Belt
		matrix belt;
		belt.createIdentityMat();
		matrix beltT, beltS, beltR;
		beltT.createIdentityMat();
		beltS.createIdentityMat();
		beltR.createIdentityMat();

		beltT.createTranslateMat(0.0f, -0.165f, 0.0f);
		beltR.createRotationMatZ(1.5708);
		beltS.createScaleMat(0.01f, 0.15f, 0.1f);

		belt = beltT * beltR * beltS;
		glUniformMatrix4fv(prog.getUniform("M"), 1, GL_FALSE, &belt.M[0][0]);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, (void*)0);



		// Belt Buckle
		matrix buckle;
		buckle.createIdentityMat();
		matrix buckleT, buckleS, buckleR;
		buckleT.createIdentityMat();
		buckleS.createIdentityMat();
		buckleR.createIdentityMat();

		buckleT.createTranslateMat(0.0f, -0.165f, 0.11f);
		buckleR.createRotationMatY(1.5708);
		buckleS.createScaleMat(0.005f, 0.005f, 0.02f);

		buckle = buckleT * buckleR * buckleS;
		glUniformMatrix4fv(prog.getUniform("M"), 1, GL_FALSE, &buckle.M[0][0]);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, (void*)0);



		// Leg 1 
		matrix leg1; 
		leg1.createIdentityMat();
		matrix leg1T, leg1S, leg1R;
		leg1T.createIdentityMat();
		leg1S.createIdentityMat();
		leg1R.createIdentityMat();

		leg1T.createTranslateMat(-0.1f, -0.325f, 0.0f);
		leg1S.createScaleMat(0.03f, 0.15f, 0.05f);
		leg1R.createRotationMatY(0.785398);

		leg1 = leg1T * leg1R * leg1S;
		glUniformMatrix4fv(prog.getUniform("M"), 1, GL_FALSE, &leg1.M[0][0]);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, (void*)0);

		// Leg 2
		matrix leg2;
		leg2.createIdentityMat();
		matrix leg2T, leg2S, leg2R;
		leg2T.createIdentityMat();
		leg2S.createIdentityMat();
		leg2R.createIdentityMat();

		leg2T.createTranslateMat(0.1f, -0.325f, 0.0f);
		leg2S.createScaleMat(0.03f, 0.15f, 0.05f);
		leg2R.createRotationMatY(0.785398);

		leg2 = leg2T * leg2R * leg2S;
		glUniformMatrix4fv(prog.getUniform("M"), 1, GL_FALSE, &leg2.M[0][0]);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, (void*)0);


		if (armRotation == 1 && LeftArm <= 1.55f)
		{
			LeftArm += 0.01;
		}

		if (armRotationDown == 1 && LeftArm >= -0.7f)
		{
			LeftArm -= 0.01;
		}

		// Lower Arm Left
		matrix armL;
		armL.createIdentityMat();
		matrix armLT, armLTO, armLS, armLR;
		armLT.createIdentityMat();
		armLTO.createIdentityMat();
		armLS.createIdentityMat();
		armLR.createIdentityMat();

		armLT.createTranslateMat(-0.16f, 0.1f, 0.0f);
		armLTO.createTranslateMat(0.0f, -0.07f, 0.0f);
		armLR.createRotationMatZ(-0.785398 - LeftArm);
		armLS.createScaleMat(0.01, 0.08, 0.01);


		armL = armLT * armLR * armLTO * armLS;
		matrix left_arm;
		left_arm.createIdentityMat();
		left_arm = armLT * armLR * armLTO;
		glUniformMatrix4fv(prog.getUniform("M"), 1, GL_FALSE, &armL.M[0][0]);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, (void*)0);

		if (isShake == 1)
		{
			Shake += 0.01;
			if (Shake >= 0.5f)
			{
				isShake = -1;
			}
		}
		if (isShake == -1)
		{

			Shake -= 0.01;
			if (Shake <= 0.0f)
			{
				isShake = 1;
			}
		}
		
		if (isShake == 0)
		{
			Shake = 0;
		}
		// Upper Arm Left

		matrix armL2;
		armL2.createIdentityMat();
		matrix armL2T, armL2TO, armL2S, armL2R;
		armL2T.createIdentityMat();
		armL2TO.createIdentityMat();
		armL2S.createIdentityMat();
		armL2R.createIdentityMat();

		armL2T.createTranslateMat(-0.01f, -0.08f, 0.0f);
		armL2TO.createTranslateMat(0.0f, 0.07f, 0.0f);
		armL2R.createRotationMatZ(1.309 + Shake);
		armL2S.createScaleMat(0.01, 0.08, 0.01);

		armL2 = left_arm * armL2T * armL2R * armL2TO * armL2S;
		matrix left_upper;
		left_upper = left_arm * armL2T * armL2R * armL2TO;
		glUniformMatrix4fv(prog.getUniform("M"), 1, GL_FALSE, &armL2.M[0][0]);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, (void*)0);

		// Lower Arm Right
		matrix armR;
		armR.createIdentityMat();
		matrix armRT, armRTO, armRS, armRR;
		armRT.createIdentityMat();
		armRTO.createIdentityMat();
		armRS.createIdentityMat();
		armRR.createIdentityMat();

		armRT.createTranslateMat(0.16f, 0.1f, 0.0f);
		armRTO.createTranslateMat(0.0f, -0.07f, 0.0f);
		armRR.createRotationMatZ(0.785398 + LeftArm);
		/*armRR.createRotationMatZ(0.785398);*/
		armRS.createScaleMat(0.01, 0.08, 0.01);


		armR = armRT * armRR * armRTO * armRS;
		matrix right_arm;
		right_arm = armRT * armRR * armRTO;
		glUniformMatrix4fv(prog.getUniform("M"), 1, GL_FALSE, &armR.M[0][0]);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, (void*)0);
		

		// Upper Arm Right
		matrix armR2;
		armR2.createIdentityMat();
		matrix armR2T, armR2TO, armR2S, armR2R;
		armR2T.createIdentityMat();
		armR2TO.createIdentityMat();
		armR2S.createIdentityMat();
		armR2R.createIdentityMat();

		armR2T.createTranslateMat(0.01f, -0.08f, 0.0f);
		armR2TO.createTranslateMat(0.0f, 0.07f, 0.0f);
		armR2R.createRotationMatZ(-1.309 - Shake);
		armR2S.createScaleMat(0.01, 0.08, 0.01);

		armR2 = right_arm * armR2T * armR2R * armR2TO * armR2S;
		matrix right_upper = right_arm * armR2T * armR2R * armR2TO;
		glUniformMatrix4fv(prog.getUniform("M"), 1, GL_FALSE, &armR2.M[0][0]);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, (void*)0);
		
		
		// Left Hand

		matrix H1;
		H1.createIdentityMat();
		matrix H1T, H1S, H1R;
		H1T.createIdentityMat();
		H1S.createIdentityMat();
		
		H1T.createTranslateMat(-0.001f, 0.095f, 0.0f);
		H1S.createScaleMat(0.01, 0.012, 0.01);
		
		H1 = left_upper * H1T * H1S;
		glUniformMatrix4fv(prog.getUniform("M"), 1, GL_FALSE, &H1.M[0][0]);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, (void*)0);


		// Thumb 1 

		matrix Thumb1;
		Thumb1.createIdentityMat();
		matrix Thumb1T, Thumb1S;
		Thumb1T.createIdentityMat();
		Thumb1S.createIdentityMat();

		Thumb1T.createTranslateMat(0.015f, 0.09f, 0.0f);
		Thumb1S.createScaleMat(0.01, 0.003, 0.01);
		
		Thumb1 = left_upper * Thumb1T * Thumb1S;
		glUniformMatrix4fv(prog.getUniform("M"), 1, GL_FALSE, &Thumb1.M[0][0]);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, (void*)0);

		// Left Index Finger

		matrix IndexL;
		IndexL.createIdentityMat();
		matrix IndexLT, IndexLS;
		IndexLT.createIdentityMat();
		IndexLS.createIdentityMat();

		IndexLT.createTranslateMat(0.007f, 0.117f, 0.0f);
		IndexLS.createScaleMat(0.002, 0.012, 0.01);
		
		IndexL = left_upper * IndexLT * IndexLS;
		glUniformMatrix4fv(prog.getUniform("M"), 1, GL_FALSE, &IndexL.M[0][0]);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, (void*)0);
		
		
		// Left Middle Finger

		matrix MiddleL;
		MiddleL.createIdentityMat();
		matrix MiddleLT, MiddleLS;
		MiddleLT.createIdentityMat();
		MiddleLS.createIdentityMat();

		MiddleLT.createTranslateMat(0.002f, 0.117f, 0.0f);
		MiddleLS.createScaleMat(0.002, 0.012, 0.01);

		MiddleL = left_upper * MiddleLT * MiddleLS;
		glUniformMatrix4fv(prog.getUniform("M"), 1, GL_FALSE, &MiddleL.M[0][0]);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, (void*)0);

		

		// Left Ring Finger
		
		matrix RingL;
		RingL.createIdentityMat();
		matrix RingLT, RingLS;
		RingLT.createIdentityMat();
		RingLS.createIdentityMat();

		RingLT.createTranslateMat(-0.003f, 0.117f, 0.0f);
		RingLS.createScaleMat(0.002, 0.012, 0.01);
		RingL = left_upper * RingLT * RingLS;
		glUniformMatrix4fv(prog.getUniform("M"), 1, GL_FALSE, &RingL.M[0][0]);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, (void*)0);


		// Left Pinkey Finger
		
		matrix PL;
		PL.createIdentityMat();
		matrix PLT, PLS;
		PLT.createIdentityMat();
		PLS.createIdentityMat();

		PLT.createTranslateMat(-0.008f, 0.117f, 0.0f);
		PLS.createScaleMat(0.002, 0.012, 0.01);
		PL = left_upper * PLT * PLS;
		glUniformMatrix4fv(prog.getUniform("M"), 1, GL_FALSE, &PL.M[0][0]);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, (void*)0);

		// Right Hand

		matrix H2;
		H2.createIdentityMat();
		matrix H2T, H2S, H2R;
		H2T.createIdentityMat();
		H2S.createIdentityMat();

		H2T.createTranslateMat(0.001f, 0.095f, 0.0f);
		H2S.createScaleMat(0.01, 0.012, 0.01);

		H2 = right_upper* H2T * H2S;
		glUniformMatrix4fv(prog.getUniform("M"), 1, GL_FALSE, &H2.M[0][0]);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, (void*)0);



		// Thumb 2

		matrix Thumb2;
		Thumb2.createIdentityMat();
		matrix Thumb2T, Thumb2S;
		Thumb2T.createIdentityMat();
		Thumb2S.createIdentityMat();

		Thumb2T.createTranslateMat(-0.015f, 0.09f, 0.0f);
		Thumb2S.createScaleMat(0.01, 0.003, 0.01);

		Thumb2 = right_upper * Thumb2T * Thumb2S;
		glUniformMatrix4fv(prog.getUniform("M"), 1, GL_FALSE, &Thumb2.M[0][0]);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, (void*)0);




		// Right Index Finger

		matrix IndexR;
		IndexR.createIdentityMat();
		matrix IndexRT, IndexRS;
		IndexRT.createIdentityMat();
		IndexRS.createIdentityMat();

		IndexRT.createTranslateMat(-0.007f, 0.117f, 0.0f);
		IndexRS.createScaleMat(0.002, 0.012, 0.01);

		IndexR = right_upper * IndexRT * IndexRS;
		glUniformMatrix4fv(prog.getUniform("M"), 1, GL_FALSE, &IndexR.M[0][0]);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, (void*)0);


		// Right Middle Finger

		matrix MiddleR;
		MiddleR.createIdentityMat();
		matrix MiddleRT, MiddleRS;
		MiddleRT.createIdentityMat();
		MiddleRS.createIdentityMat();

		MiddleRT.createTranslateMat(-0.002f, 0.117f, 0.0f);
		MiddleRS.createScaleMat(0.002, 0.012, 0.01);

		MiddleR = right_upper * MiddleRT * MiddleRS;
		glUniformMatrix4fv(prog.getUniform("M"), 1, GL_FALSE, &MiddleR.M[0][0]);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, (void*)0);



		// Right Ring Finger

		matrix RingR;
		RingR.createIdentityMat();
		matrix RingRT, RingRS;
		RingRT.createIdentityMat();
		RingRS.createIdentityMat();

		RingRT.createTranslateMat(0.003f, 0.117f, 0.0f);
		RingRS.createScaleMat(0.002, 0.012, 0.01);
		RingR = right_upper * RingRT * RingRS;
		glUniformMatrix4fv(prog.getUniform("M"), 1, GL_FALSE, &RingR.M[0][0]);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, (void*)0);



		// Right Pinkey Finger

		matrix PR;
		PR.createIdentityMat();
		matrix PRT, PRS;
		PRT.createIdentityMat();
		PRS.createIdentityMat();

		PRT.createTranslateMat(0.008f, 0.117f, 0.0f);
		PRS.createScaleMat(0.002, 0.012, 0.01);
		PR = right_upper * PRT * PRS;
		glUniformMatrix4fv(prog.getUniform("M"), 1, GL_FALSE, &PR.M[0][0]);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, (void*)0);
		glBindVertexArray(0);



		prog.unbind();

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
