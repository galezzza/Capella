#include "glew.h"
#include <GLFW/glfw3.h>
#include "glm.hpp"
#include "ext.hpp"
#include <iostream>
#include <cmath>

#include "Shader_Loader.h"
#include "Render_Utils.h"
#include "Camera.h"

#include "Box.cpp"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <string>



GLuint program;
Core::Shader_Loader shaderLoader;

Core::RenderContext shipContext;
Core::RenderContext n_shipContext;
Core::RenderContext sphereContext;
Core::RenderContext asteroid;

glm::vec3 cameraPos = glm::vec3(-20.f, 0, 10.f);
glm::vec3 cameraDir = glm::vec3(1.f, -0.f, 0.f);

//glm::vec3 spaceshipPos = glm::vec3(-15.f, -3.f, 0);
glm::vec3 spaceshipPos = cameraPos + 1.5 * cameraDir + glm::vec3(0, -0.5f, 0);
glm::vec3 spaceshipDir = glm::vec3(1.f, -0.f, 0.f);
GLuint VAO,VBO;

float aspectRatio = 1.f;

float lastFrameTime = 0.f;
float deltaTime;

float angleSpeed;
float moveSpeed;

glm::vec3 lightColor = glm::vec3(0.9, 0.7, 0.8);

glm::mat4 createCameraMatrix()
{
	glm::vec3 cameraSide = glm::normalize(glm::cross(cameraDir,glm::vec3(0.f,1.f,0.f)));
	glm::vec3 cameraUp = glm::normalize(glm::cross(cameraSide,cameraDir));
	//glm::vec3 cameraUp = glm::vec3(0.f, 1.f, 0.f);
	glm::mat4 cameraRotrationMatrix = glm::mat4({
		cameraSide.x,cameraSide.y,cameraSide.z,0,
		cameraUp.x,cameraUp.y,cameraUp.z ,0,
		-cameraDir.x,-cameraDir.y,-cameraDir.z,0,
		0.,0.,0.,1.,
		});

	cameraRotrationMatrix = glm::transpose(cameraRotrationMatrix);

	glm::mat4 cameraMatrix = cameraRotrationMatrix * glm::translate(-cameraPos);

	//cameraMatrix = glm::mat4({
	//	1.,0.,0.,cameraPos.x,
	//	0.,1.,0.,cameraPos.y,
	//	0.,0.,1.,cameraPos.z,
	//	0.,0.,0.,1.,
	//	});

	//cameraMatrix = glm::transpose(cameraMatrix);
	//return Core::createViewMatrix(cameraPos, cameraDir, up);

	return cameraMatrix;
}

glm::mat4 createPerspectiveMatrix()
{
	
	glm::mat4 perspectiveMatrix;
	float n = 0.05;
	float f = 150.;
	float a1 = glm::min(aspectRatio, 1.f);
	float a2 = glm::min(1 / aspectRatio, 1.f);
	perspectiveMatrix = glm::mat4({
		1,0.,0.,0.,
		0.,1,0.,0.,
		0.,0.,(f+n) / (n - f),2*f * n / (n - f),
		0.,0.,-1.,0.,
		});

	
	perspectiveMatrix=glm::transpose(perspectiveMatrix);

	return perspectiveMatrix;
}

void drawObjectColor(Core::RenderContext& context, glm::mat4 modelMatrix, glm::vec3 color) {

	glm::mat4 viewProjectionMatrix = createPerspectiveMatrix() * createCameraMatrix();
	glm::mat4 transformation = viewProjectionMatrix * modelMatrix;
	glUniformMatrix4fv(glGetUniformLocation(program, "transformation"), 1, GL_FALSE, (float*)&transformation);
	glUniform3f(glGetUniformLocation(program, "color"), color.x, color.y, color.z);
	Core::DrawContext(context);
}
void renderScene(GLFWwindow* window)
{
	glClearColor(0.0f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glm::mat4 transformation;
	float time = glfwGetTime();

	int* width = new int(0);
	int* height = new int(0);
	glfwGetWindowSize(window, width, height);

	double gl_xpos, gl_ypos;
	glfwGetCursorPos(window, &gl_xpos, &gl_ypos);

	if ((0 < gl_xpos)&&(gl_xpos < *width) && (0 < gl_ypos) && (gl_ypos < *height)) {
		if (gl_xpos < *width / 2) {
			spaceshipDir = glm::vec3(glm::eulerAngleY(angleSpeed) * glm::vec4(spaceshipDir, 0));
		}
		else {
			spaceshipDir = glm::vec3(glm::eulerAngleY(-angleSpeed) * glm::vec4(spaceshipDir, 0));
		}
	}
	


	glUseProgram(program);
	//drawObjectColor(sphereContext, glm::translate(glm::vec3(0, -10, 0)) * glm::scale(glm::vec3(1000, 0.1, 1000)), glm::vec3(0.8, 0.52, 0.24));
	drawObjectColor(sphereContext, glm::translate(glm::vec3(500, 100, 270)) * glm::scale(glm::vec3(28)), glm::vec3(1, 1, 0.9));
	drawObjectColor(sphereContext, glm::translate(glm::vec3(500, 30, 370)) * glm::scale(glm::vec3(25)), glm::vec3(0.95, 0.64, 0.64));
	https://imgix.ranker.com/user_node_img/50075/1001499181/original/tatooine-photo-u1?w=650&q=50&fm=pjpg&fit=crop&crop=faces
	glClear(GL_DEPTH_BUFFER_BIT);

	glm::mat4 sunTransformation = glm::translate(glm::vec3(0, -2, 0)) * glm::scale(glm::vec3(5));
	drawObjectColor(sphereContext, sunTransformation, glm::vec3(0.9, 0.9, 0.2));
	//drawObjectColor(sphereContext, glm::translate(glm::vec3(cos(time)*2, 0, sin(time)*2)), glm::vec3(0.9, 0.9, 0.2));

	glm::mat4 earthTransformation = glm::rotate(time, glm::vec3(0, 1.0f, 0)) * glm::translate(glm::vec3(0, -2, 12.5));
	drawObjectColor(sphereContext, earthTransformation, glm::vec3(0.3, 0.8, 0.2));

	//transformation = transformation * scale(glm::vec3(0.5, 0.5, 0.5c

	glm::mat4 moonTransformation = earthTransformation*glm::rotate(time*5, glm::vec3(0, 1.0f, 0)) * glm::translate(glm::vec3(0, 0, 2))* glm::scale(glm::vec3(0.5));
	drawObjectColor(sphereContext, moonTransformation, glm::vec3(0.8, 0.8, 0.8));


	glm::mat4 marsTransformation = glm::rotate(time*0.52f , glm::vec3(0, 1.0f, 0)) * glm::translate(glm::vec3(0, -2, -34)) * glm::scale(glm::vec3(0.8));
	drawObjectColor(sphereContext, marsTransformation, glm::vec3(0.9, 0.3, 0.2));

	glm::mat4 jupiterTransformation = glm::rotate(time / 12.f, glm::vec3(0, 1.0f, 0)) * glm::translate(glm::vec3(75, -2, -75)) * glm::scale(glm::vec3(2));
	drawObjectColor(sphereContext, jupiterTransformation, glm::vec3(0.6, 0.6, 0.2));

	glm::mat4 jupiterRingsTransformation = jupiterTransformation * glm::scale(glm::vec3(1.5, 0.1, 1.5));
	drawObjectColor(sphereContext, jupiterRingsTransformation, glm::vec3(0.8, 0.8, 0.8));



	//ASTEROID BELTS	
	glm::mat4 asteroidBeltTransformation = glm::rotate(time * 0.4f, glm::vec3(0, 1.0f, 0)) * glm::translate(glm::vec3(34, -2, -34)) * glm::scale(glm::vec3(0.4)) * glm::rotate(time * 2.f, glm::vec3(0, 0, 1.0f));
	drawObjectColor(asteroid, asteroidBeltTransformation, glm::vec3(0.8, 0.8, 0.8));
	asteroidBeltTransformation = glm::rotate(time * 0.4f, glm::vec3(0, 1.0f, 0)) * glm::translate(glm::vec3(-34, -2, 34)) * glm::scale(glm::vec3(0.4)) * glm::rotate(time * 2.f, glm::vec3(0, 0, 1.0f));
	drawObjectColor(asteroid, asteroidBeltTransformation, glm::vec3(0.8, 0.8, 0.8));
	
	asteroidBeltTransformation = glm::rotate(time * 0.4f, glm::vec3(0, 1.0f, 0)) * glm::translate(glm::vec3(48, -2, -0)) * glm::scale(glm::vec3(0.4)) * glm::rotate(time * 2.f, glm::vec3(1.0f, 0, 0));
	drawObjectColor(asteroid, asteroidBeltTransformation, glm::vec3(0.8, 0.8, 0.8));
	asteroidBeltTransformation = glm::rotate(time * 0.4f, glm::vec3(0, 1.0f, 0)) * glm::translate(glm::vec3(-48, -2, 0)) * glm::scale(glm::vec3(0.4)) * glm::rotate(time * 2.f, glm::vec3(1.0f, 0, 0));
	drawObjectColor(asteroid, asteroidBeltTransformation, glm::vec3(0.8, 0.8, 0.8));
	
	asteroidBeltTransformation = glm::rotate(time * 0.4f, glm::vec3(0, 1.0f, 0)) * glm::translate(glm::vec3(-47.7, -2, 6)) * glm::scale(glm::vec3(0.4)) * glm::rotate(time * 2.f, glm::vec3(0.5f, 0, 1.0f));
	drawObjectColor(asteroid, asteroidBeltTransformation, glm::vec3(0.8, 0.8, 0.8));
	asteroidBeltTransformation = glm::rotate(time * 0.4f, glm::vec3(0, 1.0f, 0)) * glm::translate(glm::vec3(-47.7, -2, -6)) * glm::scale(glm::vec3(0.4)) * glm::rotate(time * 2.f, glm::vec3(0.5f, 0, 1.0f));
	drawObjectColor(asteroid, asteroidBeltTransformation, glm::vec3(0.8, 0.8, 0.8));
	asteroidBeltTransformation = glm::rotate(time * 0.4f, glm::vec3(0, 1.0f, 0)) * glm::translate(glm::vec3(47.7, -2, 6)) * glm::scale(glm::vec3(0.4)) * glm::rotate(time * 2.f, glm::vec3(0.5f, 0, 1.0f));
	drawObjectColor(asteroid, asteroidBeltTransformation, glm::vec3(0.8, 0.8, 0.8));
	asteroidBeltTransformation = glm::rotate(time * 0.4f, glm::vec3(0, 1.0f, 0)) * glm::translate(glm::vec3(47.7, -2, -6)) * glm::scale(glm::vec3(0.4)) * glm::rotate(time * 2.f, glm::vec3(0.5f, 0, 1.0f));
	drawObjectColor(asteroid, asteroidBeltTransformation, glm::vec3(0.8, 0.8, 0.8));

	asteroidBeltTransformation = glm::rotate(time * 0.4f, glm::vec3(0, 1.0f, 0)) * glm::translate(glm::vec3(39, -2, -27)) * glm::scale(glm::vec3(0.4)) * glm::rotate(time * 2.f, glm::vec3(-0.5f, 0, 3.0f));
	drawObjectColor(asteroid, asteroidBeltTransformation, glm::vec3(0.8, 0.8, 0.8));
	asteroidBeltTransformation = glm::rotate(time * 0.4f, glm::vec3(0, 1.0f, 0)) * glm::translate(glm::vec3(39, -2, 27)) * glm::scale(glm::vec3(0.4)) * glm::rotate(time * 2.f, glm::vec3(-0.5f, 0, 3.0f));
	drawObjectColor(asteroid, asteroidBeltTransformation, glm::vec3(0.8, 0.8, 0.8));
	asteroidBeltTransformation = glm::rotate(time * 0.4f, glm::vec3(0, 1.0f, 0)) * glm::translate(glm::vec3(-39, -2, -27)) * glm::scale(glm::vec3(0.4)) * glm::rotate(time * 2.f, glm::vec3(-0.5f, 0, 3.0f));
	drawObjectColor(asteroid, asteroidBeltTransformation, glm::vec3(0.8, 0.8, 0.8));
	asteroidBeltTransformation = glm::rotate(time * 0.4f, glm::vec3(0, 1.0f, 0)) * glm::translate(glm::vec3(-39, -2, 27)) * glm::scale(glm::vec3(0.4)) * glm::rotate(time * 2.f, glm::vec3(-0.5f, 0, 3.0f));
	drawObjectColor(asteroid, asteroidBeltTransformation, glm::vec3(0.8, 0.8, 0.8));

	asteroidBeltTransformation = glm::rotate(time * 0.4f, glm::vec3(0, 1.0f, 0)) * glm::translate(glm::vec3(-31, -2, 36)) * glm::scale(glm::vec3(0.4)) * glm::rotate(time * 2.f, glm::vec3(1.0f, 2, 0.23f));
	drawObjectColor(asteroid, asteroidBeltTransformation, glm::vec3(0.8, 0.8, 0.8));
	asteroidBeltTransformation = glm::rotate(time * 0.4f, glm::vec3(0, 1.0f, 0)) * glm::translate(glm::vec3(-31, -2, -36)) * glm::scale(glm::vec3(0.4)) * glm::rotate(time * 2.f, glm::vec3(1.0f, 2, 0.23f));
	drawObjectColor(asteroid, asteroidBeltTransformation, glm::vec3(0.8, 0.8, 0.8));
	asteroidBeltTransformation = glm::rotate(time * 0.4f, glm::vec3(0, 1.0f, 0)) * glm::translate(glm::vec3(31, -2, 36)) * glm::scale(glm::vec3(0.4)) * glm::rotate(time * 2.f, glm::vec3(1.0f, 2, 0.23f));
	drawObjectColor(asteroid, asteroidBeltTransformation, glm::vec3(0.8, 0.8, 0.8));
	asteroidBeltTransformation = glm::rotate(time * 0.4f, glm::vec3(0, 1.0f, 0)) * glm::translate(glm::vec3(31, -2, -36)) * glm::scale(glm::vec3(0.4)) * glm::rotate(time * 2.f, glm::vec3(1.0f, 2, 0.23f));
	drawObjectColor(asteroid, asteroidBeltTransformation, glm::vec3(0.8, 0.8, 0.8));

	asteroidBeltTransformation = glm::rotate(time * 0.4f, glm::vec3(0, 1.0f, 0)) * glm::translate(glm::vec3(-46.56, -2, -12)) * glm::scale(glm::vec3(0.4)) * glm::rotate(time * 2.f, glm::vec3(7.0f, -1, 0.23f));
	drawObjectColor(asteroid, asteroidBeltTransformation, glm::vec3(0.8, 0.8, 0.8));
	asteroidBeltTransformation = glm::rotate(time * 0.4f, glm::vec3(0, 1.0f, 0)) * glm::translate(glm::vec3(46.56, -2, -12)) * glm::scale(glm::vec3(0.4)) * glm::rotate(time * 2.f, glm::vec3(7.0f, -1, 0.23f));
	drawObjectColor(asteroid, asteroidBeltTransformation, glm::vec3(0.8, 0.8, 0.8));
	asteroidBeltTransformation = glm::rotate(time * 0.4f, glm::vec3(0, 1.0f, 0)) * glm::translate(glm::vec3(-46.56, -2, 12)) * glm::scale(glm::vec3(0.4)) * glm::rotate(time * 2.f, glm::vec3(7.0f, -1, 0.23f));
	drawObjectColor(asteroid, asteroidBeltTransformation, glm::vec3(0.8, 0.8, 0.8));
	asteroidBeltTransformation = glm::rotate(time * 0.4f, glm::vec3(0, 1.0f, 0)) * glm::translate(glm::vec3(46.56, -2, 12)) * glm::scale(glm::vec3(0.4)) * glm::rotate(time * 2.f, glm::vec3(7.0f, -1, 0.23f));
	drawObjectColor(asteroid, asteroidBeltTransformation, glm::vec3(0.8, 0.8, 0.8));

	asteroidBeltTransformation = glm::rotate(time * 0.4f, glm::vec3(0, 1.0f, 0)) * glm::translate(glm::vec3(44.58, -2, -18)) * glm::scale(glm::vec3(0.4)) * glm::rotate(time * 2.f, glm::vec3(-1.0f, 8, 5.23f));
	drawObjectColor(asteroid, asteroidBeltTransformation, glm::vec3(0.8, 0.8, 0.8));
	asteroidBeltTransformation = glm::rotate(time * 0.4f, glm::vec3(0, 1.0f, 0)) * glm::translate(glm::vec3(-44.58, -2, -18)) * glm::scale(glm::vec3(0.4)) * glm::rotate(time * 2.f, glm::vec3(-1.0f, 8, 5.23f));
	drawObjectColor(asteroid, asteroidBeltTransformation, glm::vec3(0.8, 0.8, 0.8));
	asteroidBeltTransformation = glm::rotate(time * 0.4f, glm::vec3(0, 1.0f, 0)) * glm::translate(glm::vec3(44.58, -2, 18)) * glm::scale(glm::vec3(0.4)) * glm::rotate(time * 2.f, glm::vec3(-1.0f, 8, 5.23f));
	drawObjectColor(asteroid, asteroidBeltTransformation, glm::vec3(0.8, 0.8, 0.8));
	asteroidBeltTransformation = glm::rotate(time * 0.4f, glm::vec3(0, 1.0f, 0)) * glm::translate(glm::vec3(-44.58, -2, 18)) * glm::scale(glm::vec3(0.4)) * glm::rotate(time * 2.f, glm::vec3(-1.0f, 8, 5.23f));
	drawObjectColor(asteroid, asteroidBeltTransformation, glm::vec3(0.8, 0.8, 0.8));

	asteroidBeltTransformation = glm::rotate(time * 0.4f, glm::vec3(0, 1.0f, 0)) * glm::translate(glm::vec3(41.66, -2, -24)) * glm::scale(glm::vec3(0.4)) * glm::rotate(time * 2.f, glm::vec3(1.0f, 2, -5.23f));
	drawObjectColor(asteroid, asteroidBeltTransformation, glm::vec3(0.8, 0.8, 0.8));
	asteroidBeltTransformation = glm::rotate(time * 0.4f, glm::vec3(0, 1.0f, 0)) * glm::translate(glm::vec3(41.66, -2, 24)) * glm::scale(glm::vec3(0.4)) * glm::rotate(time * 2.f, glm::vec3(1.0f, 2, -5.23f));
	drawObjectColor(asteroid, asteroidBeltTransformation, glm::vec3(0.8, 0.8, 0.8));
	asteroidBeltTransformation = glm::rotate(time * 0.4f, glm::vec3(0, 1.0f, 0)) * glm::translate(glm::vec3(-41.66, -2, 24)) * glm::scale(glm::vec3(0.4)) * glm::rotate(time * 2.f, glm::vec3(1.0f, 2, -5.23f));
	drawObjectColor(asteroid, asteroidBeltTransformation, glm::vec3(0.8, 0.8, 0.8));
	asteroidBeltTransformation = glm::rotate(time * 0.4f, glm::vec3(0, 1.0f, 0)) * glm::translate(glm::vec3(-41.66, -2, -24)) * glm::scale(glm::vec3(0.4)) * glm::rotate(time * 2.f, glm::vec3(1.0f, 2, -5.23f));
	drawObjectColor(asteroid, asteroidBeltTransformation, glm::vec3(0.8, 0.8, 0.8));

	asteroidBeltTransformation = glm::rotate(time * 0.4f, glm::vec3(0, 1.0f, 0)) * glm::translate(glm::vec3(-37.57, -2, -30)) * glm::scale(glm::vec3(0.4)) * glm::rotate(time * 2.f, glm::vec3(1.0f, 0.5, -0.91f));
	drawObjectColor(asteroid, asteroidBeltTransformation, glm::vec3(0.8, 0.8, 0.8));
	asteroidBeltTransformation = glm::rotate(time * 0.4f, glm::vec3(0, 1.0f, 0)) * glm::translate(glm::vec3(37.57, -2, -30)) * glm::scale(glm::vec3(0.4)) * glm::rotate(time * 2.f, glm::vec3(1.0f, 0.5, -0.91f));
	drawObjectColor(asteroid, asteroidBeltTransformation, glm::vec3(0.8, 0.8, 0.8));
	asteroidBeltTransformation = glm::rotate(time * 0.4f, glm::vec3(0, 1.0f, 0)) * glm::translate(glm::vec3(-37.57, -2, 30)) * glm::scale(glm::vec3(0.4)) * glm::rotate(time * 2.f, glm::vec3(1.0f, 0.5, -0.91f));
	drawObjectColor(asteroid, asteroidBeltTransformation, glm::vec3(0.8, 0.8, 0.8));
	asteroidBeltTransformation = glm::rotate(time * 0.4f, glm::vec3(0, 1.0f, 0)) * glm::translate(glm::vec3(37.57, -2, 30)) * glm::scale(glm::vec3(0.4)) * glm::rotate(time * 2.f, glm::vec3(1.0f, 0.5, -0.91f));
	drawObjectColor(asteroid, asteroidBeltTransformation, glm::vec3(0.8, 0.8, 0.8));


	//SPACESHIP
	glm::vec3 cameraSide = glm::normalize(glm::cross(spaceshipDir, glm::vec3(0.f, 1.f, 0.f)));
	glm::vec3 cameraUp = glm::normalize(glm::cross(cameraSide, spaceshipDir));
	glm::mat4 shipTransformation = glm::translate(spaceshipPos) * glm::rotate(0.f, glm::vec3(0, 1.0f, 0)) * glm::mat4({
																														cameraSide.x,cameraSide.y,cameraSide.z,0,
																														cameraUp.x,cameraUp.y,cameraUp.z ,0,
																														-spaceshipDir.x,-spaceshipDir.y,-spaceshipDir.z,0,
																														0.,0.,0.,1.,
																												});
	drawObjectColor(n_shipContext, shipTransformation, glm::vec3(0.1, 0.1, 0.9));

	glUseProgram(0);
	glfwSwapBuffers(window);
}
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	aspectRatio = width / float(height);
	glViewport(0, 0, width, height);
}
void loadModelToContext(std::string path, Core::RenderContext& context)
{
	Assimp::Importer import;
	const aiScene* scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_CalcTangentSpace);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cout << "ERROR::ASSIMP::" << import.GetErrorString() << std::endl;
		return;
	}
	context.initFromAssimpMesh(scene->mMeshes[0]);
}

void init(GLFWwindow* window)
{
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	glEnable(GL_DEPTH_TEST);
	program = shaderLoader.CreateProgram("shaders/shader_4_1.vert", "shaders/shader_4_1.frag");

	loadModelToContext("./models/sphere.obj",sphereContext);
	loadModelToContext("./models/spaceship.obj", shipContext);
	loadModelToContext("./models/LPP.obj", asteroid);
	loadModelToContext("./models/cosmo black rtiger roblox os.obj", n_shipContext);

}

void shutdown(GLFWwindow* window)
{
	shaderLoader.DeleteProgram(program);
}

//obsluga wejscia
void processInput(GLFWwindow* window)
{
	//glm::vec3 cameraSide = glm::normalize(glm::cross(cameraDir, glm::vec3(0.f,1.f,0.f)));
	glm::vec3 spaceshipSide = glm::normalize(glm::cross(spaceshipDir, glm::vec3(0.f,1.f,0.f)));
	//float angleSpeed = 0.05f;
	//float moveSpeed = 0.5f;

	float time = glfwGetTime();
	deltaTime = time - lastFrameTime;
	lastFrameTime = time;
	const float S = 10;
	const float R = 1;
	float fps = 1 / deltaTime;
	
	angleSpeed = R / fps;
	moveSpeed = S / fps;
	
	
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		//cameraPos = cameraPos + (cameraDir * moveSpeed);
		//spaceshipPos = cameraPos + 1.5 * cameraDir + glm::vec3(0, -0.5f, 0);
		spaceshipPos = spaceshipPos + spaceshipDir * moveSpeed;
		//cameraPos = spaceshipPos - 1.5 * spaceshipDir + glm::vec3(0, 0.5f, 0);
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		//cameraPos -= cameraDir * moveSpeed;
		spaceshipPos -= spaceshipDir * moveSpeed;
	}
	if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS){
		//cameraPos += cameraSide * moveSpeed;
		spaceshipPos += spaceshipSide * moveSpeed;
	}
	if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS) {
		//cameraPos -= cameraSide * moveSpeed;
		spaceshipPos -= spaceshipSide * moveSpeed;
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		//cameraDir = glm::vec3(glm::eulerAngleY(angleSpeed) * glm::vec4(cameraDir, 0));
		spaceshipDir = glm::vec3(glm::eulerAngleY(angleSpeed) * glm::vec4(spaceshipDir, 0));
	}	
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		//cameraDir = glm::vec3(glm::eulerAngleY(-angleSpeed) * glm::vec4(cameraDir, 0));
		spaceshipDir = glm::vec3(glm::eulerAngleY(-angleSpeed) * glm::vec4(spaceshipDir, 0));
	}
	//spaceshipPos = cameraPos + 1.5 * cameraDir + glm::vec3(0, -0.5f, 0);
	//spaceshipDir = cameraDir;
	cameraPos = spaceshipPos - 1.5 * spaceshipDir + glm::vec3(0, 0.5f, 0);
	cameraDir = spaceshipDir;

	//cameraDir = glm::normalize(-cameraPos);

}

// funkcja jest glowna petla
void renderLoop(GLFWwindow* window) {
	while (!glfwWindowShouldClose(window))
	{
		processInput(window);

		renderScene(window);
		glfwPollEvents();
	}
}
//}