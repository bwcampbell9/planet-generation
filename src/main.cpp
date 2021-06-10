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
#include <glm/gtc/quaternion.hpp>

#define PI 3.1415926536

#define DRAW_LINES false

#define RADIUS 4

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
class planet
{
public:
	glm::vec3 pos, rot;
	int w, a, s, d, shift;
	float speedConst;
	planet()
	{
		w = a = s = d = shift = 0;
		rot = glm::vec3(0, 0, 0);
		pos = glm::vec3(0, 0, 0);
	}

	glm::mat4 process(double ftime, float distFact)
	{
		//cout << distFact << "\n";

		if (shift == 1) {
			speedConst = 3;
		}
		else {
			speedConst = 1;
		}

		speedConst *= pow(distFact, .5);

		float speed = 0, elevation = 0, angle = 0, vJump = 0;
		float xangle = 0;
		if (w == 1)
		{
			xangle = speedConst * ftime;
		}
		else if (s == 1)
		{
			xangle = -speedConst * ftime;
		}
		float yangle = 0;
		if (a == 1)
			yangle = speedConst * ftime;
		else if (d == 1)
			yangle = -speedConst * ftime;

		rot.y += yangle;
		rot.x += xangle;

		vec3 axis = cross(vec3(0, 0, 1), rot);

		glm::mat4 T = glm::translate(glm::mat4(1), pos);
		return T;
	}
};
planet myplanet;

class camera
{
public:
	glm::vec3 pos, rot;
	int in, out, q, e, up, down, left, right, shift, timeAdd, timeSub;
	float speedConst;
	float rotSpeed;
	camera()
	{
		in = out = q = e = up = down = shift = timeAdd = timeSub = 0;
		speedConst = 5;
		rotSpeed = .01;
		rot = glm::vec3(0, 0, 0);
		pos = glm::vec3(0, 0, -10);
	}
	glm::mat4 process(double ftime, float distFact)
	{
		//cout << distFact << "\n";

		if (shift == 1) {
			speedConst = 50;
		}
		else {
			speedConst = 5;
		}

		speedConst *= distFact;

		float speed = 0, elevation = 0, angle = 0, vJump = 0;
		if (in == 1)
		{
			speed = speedConst * ftime;
		}
		else if (out == 1)
		{
			speed = -speedConst * ftime;
		}
		/*float yangle = 0;
		if (a == 1)
			yangle = -3 * ftime;
		else if (d == 1)
			yangle = 3 * ftime;*/

		if (up == 1) {
			rot.x += rotSpeed;
		}
		else if (down == 1) {
			rot.x -= rotSpeed;
		}
		if (left == 1) {
			rot.y += rotSpeed;
		}
		else if (right == 1) {
			rot.y -= rotSpeed;
		}

		if (q == 1) {
			elevation = speedConst * ftime;
		}
		else if (e == 1) {
			elevation = -speedConst * ftime;
		}

		pos.z += speed;
		return glm::lookAt(pos, pos + vec3(cos(rot.x)*sin(rot.y), sin(rot.x), cos(rot.x) * cos(rot.y)), vec3(0, 1, 0));// vec3(sin(rot.x), sin(rot.y), 0), vec3(0, 1, 0));
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
	GLuint BiomeTexture;

	GLuint VertexArrayID, VertexBufferID, VertexColorIDBox, IndexBufferIDBox;

	float time = 0.0;
	bool morph = false;

	void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
	{
		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		{
			glfwSetWindowShouldClose(window, GL_TRUE);
		}

		if (key == GLFW_KEY_W && action == GLFW_PRESS)
		{
			myplanet.w = 1;
		}
		if (key == GLFW_KEY_W && action == GLFW_RELEASE)
		{
			myplanet.w = 0;
		}
		if (key == GLFW_KEY_S && action == GLFW_PRESS)
		{
			myplanet.s = 1;
		}
		if (key == GLFW_KEY_S && action == GLFW_RELEASE)
		{
			myplanet.s = 0;
		}
		if (key == GLFW_KEY_A && action == GLFW_PRESS)
		{
			myplanet.a = 1;
		}
		if (key == GLFW_KEY_A && action == GLFW_RELEASE)
		{
			myplanet.a = 0;
		}
		if (key == GLFW_KEY_D && action == GLFW_PRESS)
		{
			myplanet.d = 1;
		}
		if (key == GLFW_KEY_D && action == GLFW_RELEASE)
		{
			myplanet.d = 0;
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
			myplanet.shift = 1;
		}
		if (key == GLFW_KEY_LEFT_SHIFT && action == GLFW_RELEASE)
		{
			mycam.shift = 0;
			myplanet.shift = 0;
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
		
		if (key == GLFW_KEY_LEFT && action == GLFW_PRESS)
		{
			mycam.left = 1;
		}
		if (key == GLFW_KEY_LEFT && action == GLFW_RELEASE)
		{
			mycam.left = 0;
		}

		if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS)
		{
			mycam.right = 1;
		}
		if (key == GLFW_KEY_RIGHT && action == GLFW_RELEASE)
		{
			mycam.right = 0;
		}

		if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
			morph = true;
		}

	}

	// callback for the mouse when clicked move the triangle when helper functions
	// written
	void mouseCallback(GLFWwindow *window, int button, int action, int mods)
	{
		if (button == GLFW_MOUSE_BUTTON_1 && action == GLFW_PRESS) {
			mycam.in = 1;
		}
		if (button == GLFW_MOUSE_BUTTON_1 && action == GLFW_RELEASE) {
			mycam.in = 0;
		}
		if (button == GLFW_MOUSE_BUTTON_2 && action == GLFW_PRESS) {
			mycam.out = 1;
		}
		if (button == GLFW_MOUSE_BUTTON_2 && action == GLFW_RELEASE) {
			mycam.out = 0;
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
		
		// Biome Texture
		str = resourceDirectory + "/biomes.png";
		strcpy(filepath, str.c_str());
		data = stbi_load(filepath, &width, &height, &channels, 4);
		glGenTextures(1, &BiomeTexture);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, BiomeTexture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);


		//[TWOTEXTURES]
		//set the 2 textures to the correct samplers in the fragment shader:
		//GLuint SkyTextureLocation;
		//GLuint GrassNormalLocation, SnowNormalLocation, SandNormalLocation, CliffNormalLocation;

		//SkyTextureLocation = glGetUniformLocation(progSky->pid, "dayTexSampler");
		//glUseProgram(progSky->pid);
		//glUniform1i(SkyTextureLocation, 0);

		GLuint BiomeTextureLocation;
		BiomeTextureLocation = glGetUniformLocation(planetshader.pid, "biomeTex");
		glUseProgram(planetshader.pid);
		glUniform1i(BiomeTextureLocation, 0);

		//generate the VAO
		glGenVertexArrays(1, &VertexArrayID);
		glBindVertexArray(VertexArrayID);


		glGenBuffers(1, &VertexBufferID);
		glBindBuffer(GL_ARRAY_BUFFER, VertexBufferID);

		//GLfloat cube_vertices[98 * 3];
		//int i = 0;
		//for (float x = 0; x <= 1; x += .25) {
		//	for (float y = 0; y <= 1; y += .25) {
		//		for (float z = 0; z <= 1; z += .25) {
		//			if (abs(x) == 1 || abs(y) == 1 || abs(z) == 1) {
		//				// a point
		//				cube_vertices[i] = x;
		//				cube_vertices[i+1] = y;
		//				cube_vertices[i+2] = z;
		//				i += 3;
		//			}
		//		}
		//	}
		//}

		GLfloat cube_vertices[] = {
			// front
			1.0, 1.0,  1.0,
			-1.0, 1.0,  1.0,
			-1.0,  -1.0,  1.0,
			1.0,  -1.0,  1.0,
			// back
			1.0, -1.0, -1.0,
			-1.0, -1.0, -1.0,
			-1.0,  1.0, -1.0,
			1.0,  1.0, -1.0,
		};

		//actually memcopy the data - only do this once
		glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_DYNAMIC_DRAW);

		//we need to set up the vertex array
		glEnableVertexAttribArray(0);
		//key function to get up how many elements to pull out at a time (3)
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		glGenBuffers(1, &IndexBufferIDBox);
		//set the current state to focus on our vertex buffer
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndexBufferIDBox);


		GLushort cube_elements[] = {
			0, 1, 2, 3,     // Front face
			7, 4, 5, 6,     // Back face
			6, 5, 2, 1,     // Left face
			7, 0, 3, 4,     // Right face
			7, 6, 1, 0,     // Top face
			3, 2, 5, 4,     // Bottom face
		};
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_elements), cube_elements, GL_STATIC_DRAW);


		glBindVertexArray(0);

	}

	//General OGL initialization - set OGL state here
	void init(const std::string& resourceDirectory)
	{
		GLSL::checkVersion();

		// Set background color.
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
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
		planetshader.addUniform("InterpFract");
		planetshader.addUniform("rotation");
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
		if (morph) {
			time += frametime;
		}
		// Get current frame buffer size.
		int width, height;
		glfwGetFramebufferSize(windowManager->getHandle(), &width, &height);
		float aspect = width / (float)height;
		glViewport(0, 0, width, height);

		// Clear framebuffer.
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Create the matrix stacks - please leave these alone for now

		glm::mat4 V, M, P; //View, Model and Perspective matrix
		float distFact = distance(mycam.pos, vec3(0, 0, 0))-3;
		V = mycam.process(frametime, distFact*.1);
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

		//glEnable(GL_CULL_FACE);
		//glCullFace(GL_FRONT);

		myplanet.process(frametime, distFact * .1);

		M = mat4(1);

		//

		/* Arrays for use with glDrawElements.  This is the data for a cube with 6 different
		 * colors at the six vertices.  (Looks kind of strange without lighting.)
		 */
		glBindVertexArray(VertexArrayID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndexBufferIDBox);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, BiomeTexture);

		glUniformMatrix4fv(planetshader.getUniform("M"), 1, GL_FALSE, &M[0][0]);
		glUniformMatrix4fv(planetshader.getUniform("P"), 1, GL_FALSE, &P[0][0]);
		glUniformMatrix4fv(planetshader.getUniform("V"), 1, GL_FALSE, &V[0][0]);
		//glUniform3fv(planetshader->getUniform("camoff"), 1, &offset[0]);
		glUniform3fv(planetshader.getUniform("campos"), 1, &mycam.pos[0]);
		if (time > 1) {
			time = 1;
		}
		glUniform1f(planetshader.getUniform("InterpFract"), time);
		mat4 Rx = rotate(mat4(1), myplanet.rot.x, { 1, 0, 0 });
		mat4 Ry = rotate(mat4(1), myplanet.rot.y, { 0, 1, 0 });
		mat4 R = Rx * Ry;
		glUniformMatrix4fv(planetshader.getUniform("rotation"), 1, GL_FALSE, &R[0][0]);

		glPatchParameteri(GL_PATCH_VERTICES, 4);
		glDrawElements(GL_PATCHES, 24, GL_UNSIGNED_SHORT, (void*)0);

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
