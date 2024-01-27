#include "glew.h"
#include <GLFW/glfw3.h>
#include "glm.hpp"
#include "ext.hpp"
#include <iostream>
#include <cmath>

#include "Shader_Loader.h"
#include "Render_Utils.h"
#include "Camera.h"
#include "Texture.h"

#include "Box.cpp"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <string>

/// <summary>
/// fist commit
/// </summary>

GLuint programPBR;
GLuint programEarthPBR;
GLuint programSun;
GLuint programSpaceship;

Core::Shader_Loader shaderLoader;

Core::RenderContext shipContext;
Core::RenderContext hugeSphereContext;
Core::RenderContext n_shipContext;
Core::RenderContext sphereContext;
Core::RenderContext asteroid;

glm::vec3 cameraPos = glm::vec3(-4000.f, 0, 10.f);
glm::vec3 cameraDir = glm::vec3(1.f, -0.f, 0.f);

glm::vec3 spaceshipPos = cameraPos + 1.5 * cameraDir + glm::vec3(0, -2.5f, 0);
glm::vec3 spaceshipDir = glm::vec3(1.f, -0.f, 0.f);
GLuint VAO, VBO;

float aspectRatio = 1.f;

float lastFrameTime = 0.f;
float deltaTime;

float angleSpeed;
float moveSpeed;

float S = 8;
float R = 1;

const float maxS = 512;
const float minS = 2;

glm::vec3 lightColor = glm::vec3(1);
glm::vec3 lightDir = glm::vec3(1.0, 0.0, 0.0);

float exp_param = 400.f;
float spotLightOn = 1.0f;

glm::vec3 spotPos = spaceshipPos;
glm::vec3 spotDir = spaceshipDir + glm::vec3(0, -0.5f, 0.f);
float angleAf = 3.14f / 2.f;

namespace texture {
	GLuint earth;
	GLuint clouds;
	GLuint moon;
	GLuint ship;

	GLuint grid;

	GLuint earthNormal;
	GLuint asteroidNormal;
	GLuint shipNormal;
	
	GLuint scratches;
	GLuint rust;
}
namespace rustediron2 {
	GLuint albedo;
	GLuint metallic;
	GLuint normal;
	GLuint roughness;
}

glm::mat4 createCameraMatrix()
{
	glm::vec3 cameraSide = glm::normalize(glm::cross(cameraDir, glm::vec3(0.f, 1.f, 0.f)));
	glm::vec3 cameraUp = glm::normalize(glm::cross(cameraSide, cameraDir));
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
	float f = 15000.;
	float a1 = glm::min(aspectRatio, 1.f);
	float a2 = glm::min(1 / aspectRatio, 1.f);
	perspectiveMatrix = glm::mat4({
		1,0.,0.,0.,
		0.,1,0.,0.,
		0.,0.,(f + n) / (n - f),2 * f * n / (n - f),
		0.,0.,-1.,0.,
		});


	perspectiveMatrix = glm::transpose(perspectiveMatrix);

	return perspectiveMatrix;
}

void drawObjectColor(Core::RenderContext& context, glm::mat4 modelMatrix, glm::vec3 color, GLuint program) {

	glUseProgram(program);
	glm::mat4 viewProjectionMatrix = createPerspectiveMatrix() * createCameraMatrix();
	glm::mat4 transformation = viewProjectionMatrix * modelMatrix;
	glUniformMatrix4fv(glGetUniformLocation(program, "transformation"), 1, GL_FALSE, (float*)&transformation);
	glUniform3f(glGetUniformLocation(program, "color"), color.x, color.y, color.z);

	glUniform3f(glGetUniformLocation(program, "lightDir"), lightDir.x, lightDir.y, lightDir.z);
	glUniform3f(glGetUniformLocation(program, "lightColor"), lightColor.x, lightColor.y, lightColor.z);
	glUniform3f(glGetUniformLocation(program, "cameraPos"), cameraPos.x, cameraPos.y, cameraPos.z);

	glUniformMatrix4fv(glGetUniformLocation(program, "modelMatrix"), 1, GL_FALSE, (float*)&modelMatrix);
	glUniform3f(glGetUniformLocation(program, "lightPos"), 0, -2, 0);
	glUniform1f(glGetUniformLocation(program, "exp_param"), exp_param);

	glUniform3f(glGetUniformLocation(program, "spotPos"), spotPos.x, spotPos.y, spotPos.z);
	glUniform3f(glGetUniformLocation(program, "spotDir"), spotDir.x, spotDir.y, spotDir.z);
	glUniform1f(glGetUniformLocation(program, "angleAf"), angleAf);



	Core::DrawContext(context);
}
void drawSpaceship(Core::RenderContext& context, glm::mat4 modelMatrix, GLuint textureID1, GLuint textureID2, GLuint textureID3) {
	glUseProgram(programSpaceship);
	glm::mat4 viewProjectionMatrix = createPerspectiveMatrix() * createCameraMatrix();
	glm::mat4 transformation = viewProjectionMatrix * modelMatrix;
	glUniformMatrix4fv(glGetUniformLocation(programSpaceship, "transformation"), 1, GL_FALSE, (float*)&transformation);

	glUniform3f(glGetUniformLocation(programSpaceship, "lightDir"), lightDir.x, lightDir.y, lightDir.z);
	glUniform3f(glGetUniformLocation(programSpaceship, "lightColor"), lightColor.x, lightColor.y, lightColor.z);
	glUniform3f(glGetUniformLocation(programSpaceship, "cameraPos"), cameraPos.x, cameraPos.y, cameraPos.z);

	glUniformMatrix4fv(glGetUniformLocation(programSpaceship, "modelMatrix"), 1, GL_FALSE, (float*)&modelMatrix);
	glUniform3f(glGetUniformLocation(programSpaceship, "lightPos"), 0, -2, 0);
	glUniform1f(glGetUniformLocation(programSpaceship, "exp_param"), exp_param);


	glUniform3f(glGetUniformLocation(programSpaceship, "spotPos"), spotPos.x, spotPos.y, spotPos.z);
	glUniform3f(glGetUniformLocation(programSpaceship, "spotDir"), spotDir.x, spotDir.y, spotDir.z);
	glUniform1f(glGetUniformLocation(programSpaceship, "angleAf"), angleAf);

	Core::SetActiveTexture(textureID1, "shipTexture", programSpaceship, 0);
	Core::SetActiveTexture(textureID2, "scratches", programSpaceship, 1);
	Core::SetActiveTexture(textureID3, "rust", programSpaceship, 2);

	

	Core::DrawContext(context);
}
void drawObjectColorPBR(GLuint programPBR, Core::RenderContext& context, glm::mat4 modelMatrix, GLuint textureAlbedo, GLuint textureNormal, GLuint textureMetallic, GLuint textureRoughness, GLuint textureAO) {

	glUseProgram(programPBR);
	glm::mat4 viewProjectionMatrix = createPerspectiveMatrix() * createCameraMatrix();
	glm::mat4 transformation = viewProjectionMatrix * modelMatrix;
	glUniformMatrix4fv(glGetUniformLocation(programPBR, "transformation"), 1, GL_FALSE, (float*)&transformation);
	glUniformMatrix4fv(glGetUniformLocation(programPBR, "modelMatrix"), 1, GL_FALSE, (float*)&modelMatrix);

	glUniform3f(glGetUniformLocation(programPBR, "lightPos"), 0, 0, 0);
	//glUniform3f(glGetUniformLocation(programPBR, "lightDir"), lightDir.x, lightDir.y, lightDir.z);
	//glUniform3f(glGetUniformLocation(programPBR, "lightColor"), lightColor.x, lightColor.y, lightColor.z);
	glUniform3f(glGetUniformLocation(programPBR, "cameraPos"), cameraPos.x, cameraPos.y, cameraPos.z);
	
	glUniform1f(glGetUniformLocation(programPBR, "exp_param"), exp_param);
	glUniform1f(glGetUniformLocation(programPBR, "spotLightOn"), spotLightOn);

	glUniform3f(glGetUniformLocation(programPBR, "spotPos"), spotPos.x, spotPos.y, spotPos.z);
	glUniform3f(glGetUniformLocation(programPBR, "spotDir"), spotDir.x, spotDir.y, spotDir.z);
	//glUniform1f(glGetUniformLocation(programPBR, "angleAf"), angleAf);



	Core::SetActiveTexture(textureAlbedo, "textureAlbedo", programPBR, 0);
	Core::SetActiveTexture(textureNormal, "textureNormal", programPBR, 1);
	Core::SetActiveTexture(textureMetallic, "textureMetallic", programPBR, 2);
	Core::SetActiveTexture(textureRoughness, "textureRoughness", programPBR, 3);
	Core::SetActiveTexture(textureAO, "textureAO", programPBR, 4);

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

	glClear(GL_DEPTH_BUFFER_BIT);

	//SUN
	glm::mat4 sunTransformation = glm::scale(glm::vec3(120));
	drawObjectColor(sphereContext, sunTransformation, glm::vec3(0.9, 0.9, 0.2), programSun);

	//PLANET
	glm::mat4 earthTransformation = glm::rotate(float(time * 0.001), glm::vec3(0, 1.0f, 0)) * glm::translate(glm::vec3(-2252.5, -2, 0)) * glm::scale(glm::vec3(25)) * glm::rotate(-float(time*0.125), glm::vec3(0, 1.0f, 0));
	drawObjectColorPBR(programEarthPBR, sphereContext, earthTransformation, rustediron2::albedo, rustediron2::normal, rustediron2::metallic, rustediron2::roughness, texture::clouds);
	
	glm::mat4 moonTransformation = earthTransformation * glm::rotate(time * 5, glm::vec3(0, 1.0f, 0)) * glm::translate(glm::vec3(0, 0, 2)) * glm::scale(glm::vec3(0.5));

	//SPACESHIP
	glm::vec3 cameraSide = glm::normalize(glm::cross(spaceshipDir, glm::vec3(0.f, 1.f, 0.f)));
	glm::vec3 cameraUp = glm::normalize(glm::cross(cameraSide, spaceshipDir));
	glm::mat4 shipTransformation = glm::translate(spaceshipPos) * glm::rotate(0.f, glm::vec3(0, 1.0f, 0)) * glm::mat4(	{
																														cameraSide.x,cameraSide.y,cameraSide.z,0,
																														cameraUp.x,cameraUp.y,cameraUp.z ,0,
																														spaceshipDir.x,spaceshipDir.y,spaceshipDir.z,0,
																														0.,0.,0.,1.,
																														}	);
	//drawSpaceship(shipContext, shipTransformation, texture::ship, texture::scratches, texture::rust);
	drawObjectColorPBR(programPBR, shipContext, shipTransformation, rustediron2::albedo, rustediron2::normal, rustediron2::metallic, rustediron2::roughness, texture::clouds);

	//SPOTLIGHT
	glm::mat4 spotTransformation = glm::translate(spotPos) * glm::rotate(0.f, glm::vec3(0, 1.0f, 0)) * glm::mat4({
																														cameraSide.x,cameraSide.y,cameraSide.z,0,
																														cameraUp.x,cameraUp.y,cameraUp.z ,0,
																														spotDir.x,spotDir.y,spotDir.z,0,
																														0.,0.,0.,1.,
		}
	) * glm::scale(glm::vec3(0.2));
	drawSpaceship(shipContext, spotTransformation, texture::ship, texture::scratches, texture::rust);


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
	programSun = shaderLoader.CreateProgram("shaders/shader_5_sun.vert", "shaders/shader_5_sun.frag");
	programPBR = shaderLoader.CreateProgram("shaders/shader_PBR.vert", "shaders/shader_PBR.frag");
	programEarthPBR = shaderLoader.CreateProgram("shaders/shader_earthPBR.vert", "shaders/shader_PBR.frag");
	programSpaceship = shaderLoader.CreateProgram("shaders/shader_spaceship.vert", "shaders/shader_spaceship.frag");

	loadModelToContext("./models/sphere.obj", sphereContext);
	loadModelToContext("./models/spaceship.obj", shipContext);
	loadModelToContext("./models/something.obj", hugeSphereContext);

	texture::earth = Core::LoadTexture("./textures/earth.png");
	texture::clouds = Core::LoadTexture("./textures/clouds.jpg");
	texture::moon = Core::LoadTexture("./textures/moon.png");
	texture::ship = Core::LoadTexture("./textures/spaceship.jpg");
	texture::grid = Core::LoadTexture("./textures/grid.png");
	texture::earthNormal = Core::LoadTexture("./textures/earth_normalmap.png");
	texture::asteroidNormal = Core::LoadTexture("./textures/moon_normals.png");
	texture::shipNormal = Core::LoadTexture("./textures/spaceship_normal.jpg");
	texture::scratches = Core::LoadTexture("./textures/scratches.jpg");
	texture::rust = Core::LoadTexture("./textures/rust.jpg");
	rustediron2::albedo = Core::LoadTexture("./textures/rustediron1-alt2-bl/rustediron2_basecolor.png");
	rustediron2::normal = Core::LoadTexture("./textures/rustediron1-alt2-bl/rustediron2_normal.png");
	rustediron2::metallic = Core::LoadTexture("./textures/rustediron1-alt2-bl/rustediron2_metallic.png");
	rustediron2::roughness = Core::LoadTexture("./textures/rustediron1-alt2-bl/rustediron2_roughness.png");
}

void shutdown(GLFWwindow* window)
{
	shaderLoader.DeleteProgram(programPBR);
	shaderLoader.DeleteProgram(programEarthPBR);
	shaderLoader.DeleteProgram(programSun);
	shaderLoader.DeleteProgram(programSpaceship);
}

//obsluga wejscia
void processInput(GLFWwindow* window)
{
	glm::vec3 spaceshipSide = glm::normalize(glm::cross(spaceshipDir, glm::vec3(0.f, 1.f, 0.f)));

	float time = glfwGetTime();
	deltaTime = time - lastFrameTime;
	lastFrameTime = time;
	float fps = 1 / deltaTime;

	angleSpeed = R / fps;
	moveSpeed = S / fps;


	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		spaceshipPos = spaceshipPos + spaceshipDir * moveSpeed;
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		spaceshipPos -= spaceshipDir * moveSpeed;
	}
	if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS) {
		spaceshipPos += spaceshipSide * moveSpeed;
	}
	if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS) {
		spaceshipPos -= spaceshipSide * moveSpeed;
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		spaceshipDir = glm::vec3(glm::eulerAngleY(angleSpeed) * glm::vec4(spaceshipDir, 0));
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		spaceshipDir = glm::vec3(glm::eulerAngleY(-angleSpeed) * glm::vec4(spaceshipDir, 0));
	}
	if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS) {
		if (S < maxS){
			S *= 2;
		}
		printf("%f\n", S);
	}
	if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS) {
		if (S > minS){
			S /= 2;
		}
		printf("%f\n", S);
	}
	if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS) {
		if (spotLightOn == 0.0f) {
			spotLightOn = 1.0;
		}
		else {
			spotLightOn = 0.0f;
		}
	}
	cameraPos = spaceshipPos - 1.5 * spaceshipDir + glm::vec3(0, 0.5f, 0);
	cameraDir = spaceshipDir;

	//cameraDir = glm::normalize(-cameraPos);

	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {
		exp_param += 10;
	}
	if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) {
		exp_param -= 10;
	}

	spotPos = spaceshipPos - glm::vec3(glm::normalize(spaceshipDir).x, -1.f, glm::normalize(spaceshipDir).z);
	spotDir = spotLightOn * (spaceshipDir + glm::vec3(0, -0.5f, 0));
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