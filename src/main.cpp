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

#define DRAW_LINES true

using namespace std;
using namespace glm;
shared_ptr<Shape> skySphere;


double get_last_elapsed_time()
{
	static double lasttime = glfwGetTime();
	double actualtime = glfwGetTime();
	double difference = actualtime - lasttime;
	lasttime = actualtime;
	return difference;
}
class camera
{
public:
	glm::vec3 pos, rot;
	int w, a, s, d, q, e, up, down, shift, timeAdd, timeSub;
	int speedConst;
	camera()
	{
		w = a = s = d = q = e = up = down = shift = timeAdd = timeSub = 0;
		speedConst = 5;
		rot = glm::vec3(0, 0, 0);
		pos = glm::vec3(0, 0, -10);
	}
	glm::mat4 process(double ftime)
	{
		if (shift == 1) {
			speedConst = 50;
		}
		else {
			speedConst = 5;
		}

		float speed = 0, elevation = 0, angle = 0, vJump = 0;
		if (w == 1)
		{
			speed = speedConst * ftime;
		}
		else if (s == 1)
		{
			speed = -speedConst * ftime;
		}
		float yangle = 0;
		if (a == 1)
			yangle = -3 * ftime;
		else if (d == 1)
			yangle = 3 * ftime;

		if (q == 1) {
			elevation = speedConst * ftime;
		}
		else if (e == 1) {
			elevation = -speedConst * ftime;
		}

		rot.y += yangle;
		glm::mat4 R = glm::rotate(glm::mat4(1), rot.y, glm::vec3(0, 1, 0));
		glm::vec4 dir = glm::vec4(0, elevation, speed, 1);
		dir = dir * R;
		pos += glm::vec3(dir.x, dir.y, dir.z);

		glm::mat4 T = glm::translate(glm::mat4(1), pos);
		glm::mat4 LD = glm::rotate(glm::mat4(1), angle, glm::vec3(cos(rot.x), 0, sin(rot.z)));
		return LD * R * T;
	}
};
camera mycam;

class Application : public EventCallbacks
{

public:

	WindowManager * windowManager = nullptr;

	// Our shader program
	Program planetshader;

	//texture data
	GLuint SkyTexture;

	GLuint VertexArrayID, VertexBufferID, VertexColorIDBox, IndexBufferIDBox;

	float time = 1.0;

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
		if (key == GLFW_KEY_Q && action == GLFW_PRESS)
		{
			mycam.q = 1;
		}
		if (key == GLFW_KEY_Q && action == GLFW_RELEASE)
		{
			mycam.q = 0;
		}
		if (key == GLFW_KEY_E && action == GLFW_PRESS)
		{
			mycam.e = 1;
		}
		if (key == GLFW_KEY_E && action == GLFW_RELEASE)
		{
			mycam.e = 0;
		}
		if (key == GLFW_KEY_LEFT_SHIFT && action == GLFW_PRESS)
		{
			mycam.shift = 1;
		}
		if (key == GLFW_KEY_LEFT_SHIFT && action == GLFW_RELEASE)
		{
			mycam.shift = 0;
		}

		if (key == GLFW_KEY_EQUAL && action == GLFW_PRESS)
		{
			mycam.timeAdd = 1;
		}
		if (key == GLFW_KEY_EQUAL && action == GLFW_RELEASE)
		{
			mycam.timeAdd = 0;
		}

		if (key == GLFW_KEY_MINUS && action == GLFW_PRESS)
		{
			mycam.timeSub = 1;
		}
		if (key == GLFW_KEY_MINUS && action == GLFW_RELEASE)
		{
			mycam.timeSub = 0;
		}

		if (key == GLFW_KEY_UP && action == GLFW_PRESS)
		{
			mycam.up = 1;
		}
		if (key == GLFW_KEY_UP && action == GLFW_RELEASE)
		{
			mycam.up = 0;
		}

		if (key == GLFW_KEY_DOWN && action == GLFW_PRESS)
		{
			mycam.down = 1;
		}
		if (key == GLFW_KEY_DOWN && action == GLFW_RELEASE)
		{
			mycam.down = 0;
		}

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

		string resourceDirectory = "../resources";
		// Initialize mesh.
		skySphere = make_shared<Shape>();
		skySphere->loadMesh(resourceDirectory + "/sphere.obj");
		skySphere->resize();
		skySphere->init();

		int width, height, channels;
		char filepath[1000];

		string str;
		unsigned char* data;

		// Skybox Texture
		//str = resourceDirectory + "/sky.jpg";
		//strcpy(filepath, str.c_str());
		//data = stbi_load(filepath, &width, &height, &channels, 4);
		//glGenTextures(1, &SkyTexture);
		//glActiveTexture(GL_TEXTURE1);
		//glBindTexture(GL_TEXTURE_2D, SkyTexture);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		//glGenerateMipmap(GL_TEXTURE_2D);



		//[TWOTEXTURES]
		//set the 2 textures to the correct samplers in the fragment shader:
		//GLuint SkyTextureLocation;
		//GLuint GrassNormalLocation, SnowNormalLocation, SandNormalLocation, CliffNormalLocation;

		//SkyTextureLocation = glGetUniformLocation(progSky->pid, "dayTexSampler");
		//glUseProgram(progSky->pid);
		//glUniform1i(SkyTextureLocation, 0);

		//generate the VAO
		glGenVertexArrays(1, &VertexArrayID);
		glBindVertexArray(VertexArrayID);


		glGenBuffers(1, &VertexBufferID);
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
			1.0, 0.0, 0.0,
			0.0, 1.0, 0.0,
			0.0, 0.0, 1.0,
			1.0, 1.0, 1.0,
			// back colors
			1.0, 0.0, 0.0,
			0.0, 1.0, 0.0,
			0.0, 0.0, 1.0,
			1.0, 1.0, 1.0,
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
			// top
			1, 5, 6,
			6, 2, 1,
			// back
			7, 6, 5,
			5, 4, 7,
			// bottom
			4, 0, 3,
			3, 7, 4,
			// left
			4, 5, 1,
			1, 0, 4,
			// right
			3, 2, 6,
			6, 7, 3,
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
		planetshader.setVerbose(true);
		planetshader.setShaderNames(resourceDirectory + "/planet_vertex.glsl", resourceDirectory + "/planet_frag.glsl", resourceDirectory + "/tesscontrol.glsl", resourceDirectory + "/tesseval.glsl");
		if (!planetshader.init())
		{
			std::cerr << "planet shaders failed to compile... exiting!" << std::endl;
			int hold;
			cin >> hold;
			exit(1);
		}
		planetshader.addUniform("P");
		planetshader.addUniform("V");
		planetshader.addUniform("M");
		planetshader.addUniform("camoff");
		planetshader.addUniform("campos");
		planetshader.addAttribute("vertPos");
		planetshader.addAttribute("vertColor");

		// Initialize the GLSL progSkyram.
		/*progSky = std::make_shared<Program>();
		progSky->setVerbose(true);
		progSky->setShaderNames(resourceDirectory + "/skyvertex.glsl", resourceDirectory + "/skyfrag.glsl");
		if (!progSky->init())
		{
			std::cerr << "Skybox shaders failed to compile... exiting!" << std::endl;
			int hold;
			cin >> hold;
			exit(1);
		}
		progSky->addUniform("P");
		progSky->addUniform("V");
		progSky->addUniform("M");
		progSky->addUniform("campos");
		progSky->addUniform("time");
		progSky->addAttribute("vertPos");
		progSky->addAttribute("vertTex");*/
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
		float aspect = width / (float)height;
		glViewport(0, 0, width, height);

		// Clear framebuffer.
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Create the matrix stacks - please leave these alone for now

		glm::mat4 V, M, P; //View, Model and Perspective matrix
		V = mycam.process(frametime);
		M = glm::mat4(1);
		// Apply orthographic projection....
		P = glm::ortho(-1 * aspect, 1 * aspect, -1.0f, 1.0f, -2.0f, 100.0f);
		if (width < height)
		{
			P = glm::ortho(-1.0f, 1.0f, -1.0f / aspect, 1.0f / aspect, -2.0f, 100.0f);
		}
		// ...but we overwrite it (optional) with a perspective projection.
		P = glm::perspective((float)(3.14159 / 4.), (float)((float)width / (float)height), 0.1f, 100.0f); //so much type casting... GLM metods are quite funny ones

		//float sangle = 3.1415926 / 2.;
		//glm::mat4 RotateX, TransXYZ, TransY, S;

		//RotateX = glm::rotate(glm::mat4(1.0f), sangle, glm::vec3(1.0f, 0.0f, 0.0f));
		//glm::vec3 camp = -mycam.pos;
		//TransXYZ = glm::translate(glm::mat4(1.0f), camp);
		//S = glm::scale(glm::mat4(1.0f), glm::vec3(3.0f, 3.0f, 3.0f));

		//M = TransXYZ * RotateX * S;

		//if (mycam.timeAdd) {
		//	time += 0.05;
		//}
		//else if (mycam.timeSub) {
		//	time -= 0.05;
		//}
		//else if (DRAW_LINES == false) {
		//	time += 0.004;
		//}

		//cout << "\rx: " << mycam.pos.x << ", y: "<< mycam.pos.y << ", z: " << mycam.pos.z << "\tvisible: " << MESHSIZE * RESOLUTION;
		//cout << "visible: " << RESOLUTION * MESHSIZE << endl;

		// GL POLYGON _________________________________________________
		if (DRAW_LINES == true) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		}
		else {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}
		// ____________________________________________________________

		// Draw the skybox ----------------------------------------------------------------
		//progSky->bind();


		//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		//send the matrices to the shaders 
		//glUniformMatrix4fv(progSky->getUniform("P"), 1, GL_FALSE, &P[0][0]);
		//glUniformMatrix4fv(progSky->getUniform("V"), 1, GL_FALSE, &V[0][0]);
		//glUniformMatrix4fv(progSky->getUniform("M"), 1, GL_FALSE, &M[0][0]);
		//glUniform3fv(progSky->getUniform("campos"), 1, &mycam.pos[0]);
		//glUniform1f(progSky->getUniform("time"), time);

		//glActiveTexture(GL_TEXTURE0);
		//glBindTexture(GL_TEXTURE_2D, SkyTexture);
		//glDisable(GL_DEPTH_TEST);
		//skySphere->draw(progSky, FALSE);

		//glEnable(GL_DEPTH_TEST);
		//glEnable(GL_BLEND);

		//progSky->unbind();
		
		// Draw the planet -----------------------------------------------------------------
		planetshader.bind();

		glEnable(GL_CULL_FACE);
		glCullFace(GL_FRONT);

		M = translate(mat4(1), vec3(0, 0, 0));
		//

		/* Arrays for use with glDrawElements.  This is the data for a cube with 6 different
		 * colors at the six vertices.  (Looks kind of strange without lighting.)
		 */
		glBindVertexArray(VertexArrayID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndexBufferIDBox);

		glUniformMatrix4fv(planetshader.getUniform("M"), 1, GL_FALSE, &M[0][0]);
		glUniformMatrix4fv(planetshader.getUniform("P"), 1, GL_FALSE, &P[0][0]);
		glUniformMatrix4fv(planetshader.getUniform("V"), 1, GL_FALSE, &V[0][0]);
		//glUniform3fv(planetshader->getUniform("camoff"), 1, &offset[0]);
		glUniform3fv(planetshader.getUniform("campos"), 1, &mycam.pos[0]);

		glPatchParameteri(GL_PATCH_VERTICES, 3);
		glDrawElements(GL_PATCHES, 36, GL_UNSIGNED_SHORT, (void*)0);

		glBindVertexArray(0);

		planetshader.unbind();

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
	while (!glfwWindowShouldClose(windowManager->getHandle()))
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
