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
#include "SOIL/SOIL.h"

using namespace glm;

/// <summary>
/// fist commit
/// </summary>

int winWidth, winHeight;

GLuint programPBR;
GLuint programEarthPBR;
GLuint programSun;
GLuint programSpaceship;
GLuint programCubeMap;

Core::Shader_Loader shaderLoader;

Core::RenderContext shipContext;
Core::RenderContext hugeSphereContext;
Core::RenderContext n_shipContext;
Core::RenderContext sphereContext;
Core::RenderContext asteroid;
Core::RenderContext cubeMapContex;

vec3 cameraPos = vec3(-4000.f, 0, 10.f);
vec3 cameraDir = vec3(1.f, -0.f, 0.f);

vec3 spaceshipPos = cameraPos + 1.5 * cameraDir + vec3(0, -2.5f, 0);
vec3 spaceshipDir = vec3(1.f, -0.f, 0.f);
GLuint VAO, VBO;

float aspectRatio = 1.f;

float lastFrameTime = 0.f;
float deltaTime;

float angleSpeed;
float moveSpeed;

float S = 8;
float R = 1;

vec3 earthPosWor;
float earth_r;
constexpr float PLANET_R = 125;
constexpr float MOON_R = 12;
constexpr float MOON_H = 200;
float SPACESHIP_H = 5;

float msf = MOON_R;

bool addGlow = false;

const float maxS = 512;
const float minS = 2;

vec3 lightColor = vec3(1);
vec3 lightDir = vec3(1.0, 0.0, 0.0);

float exp_param = 400.f;
float spotLightOn = 1.0f;

vec3 spotPos = spaceshipPos;
vec3 spotDir = spaceshipDir + vec3(0, -0.5f, 0.f);
float angleAf = 3.14f / 2.f;

constexpr int NUM_CURVE_POINTS = 30000;
std::vector<vec3> curve_points;

namespace texture {
	GLuint cubemap;
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

enum class SceneType
{
	IN_SPACE, 
	ON_PLANET,
	CURVE_FLY,
	NUM_SCENE_TYPES
};
SceneType currSceneType = SceneType::IN_SPACE;

void printVec3(vec3 v) {
	std::cout
		<< v.x << ", "
		<< v.y << ", "
		<< v.z
		<< std::endl;
}

mat4 createCameraMatrix()
{
	vec3 cameraSide = normalize(cross(cameraDir, vec3(0.f, 1.f, 0.f)));
	vec3 cameraUp = normalize(cross(cameraSide, cameraDir));
	//vec3 cameraUp = vec3(0.f, 1.f, 0.f);
	mat4 cameraRotrationMatrix = mat4({
		cameraSide.x,cameraSide.y,cameraSide.z,0,
		cameraUp.x,cameraUp.y,cameraUp.z ,0,
		-cameraDir.x,-cameraDir.y,-cameraDir.z,0,
		0.,0.,0.,1.,
		});

	cameraRotrationMatrix = transpose(cameraRotrationMatrix);

	mat4 cameraMatrix = cameraRotrationMatrix * translate(-cameraPos);

	//cameraMatrix = mat4({
	//	1.,0.,0.,cameraPos.x,
	//	0.,1.,0.,cameraPos.y,
	//	0.,0.,1.,cameraPos.z,
	//	0.,0.,0.,1.,
	//	});

	//cameraMatrix = transpose(cameraMatrix);
	//return Core::createViewMatrix(cameraPos, cameraDir, up);

	return cameraMatrix;
}

mat4 createPerspectiveMatrix()
{

	mat4 perspectiveMatrix;
	float n = 0.05;
	float f = 15000.;
	float a1 = min(aspectRatio, 1.f);
	float a2 = min(1 / aspectRatio, 1.f);
	perspectiveMatrix = mat4({
		1,0.,0.,0.,
		0.,1,0.,0.,
		0.,0.,(f + n) / (n - f),2 * f * n / (n - f),
		0.,0.,-1.,0.,
		});


	perspectiveMatrix = transpose(perspectiveMatrix);

	return perspectiveMatrix;
}

void drawObjectColor(Core::RenderContext& context, mat4 modelMatrix, vec3 color, GLuint program) {

	glUseProgram(program);
	mat4 viewProjectionMatrix = createPerspectiveMatrix() * createCameraMatrix();
	mat4 transformation = viewProjectionMatrix * modelMatrix;
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
void drawSpaceship(Core::RenderContext& context, mat4 modelMatrix, GLuint textureID1, GLuint textureID2, GLuint textureID3) {
	glUseProgram(programSpaceship);
	mat4 viewProjectionMatrix = createPerspectiveMatrix() * createCameraMatrix();
	mat4 transformation = viewProjectionMatrix * modelMatrix;
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
void drawObjectColorPBR(GLuint programPBR, Core::RenderContext& context, mat4 modelMatrix, GLuint textureAlbedo, GLuint textureNormal, GLuint textureMetallic, GLuint textureRoughness, GLuint textureAO) {

	glUseProgram(programPBR);
	mat4 viewProjectionMatrix = createPerspectiveMatrix() * createCameraMatrix();
	mat4 transformation = viewProjectionMatrix * modelMatrix;
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

	vec3 glowColor = vec3(-1);
	if (addGlow && programPBR == programEarthPBR) {
		glowColor = vec3(0, 1, 0);
	}
	glUniform3f(glGetUniformLocation(programPBR, "glowColor"), glowColor.r, glowColor.g, glowColor.b);


	Core::SetActiveTexture(textureAlbedo, "textureAlbedo", programPBR, 0);
	Core::SetActiveTexture(textureNormal, "textureNormal", programPBR, 1);
	Core::SetActiveTexture(textureMetallic, "textureMetallic", programPBR, 2);
	Core::SetActiveTexture(textureRoughness, "textureRoughness", programPBR, 3);
	Core::SetActiveTexture(textureAO, "textureAO", programPBR, 4);

	Core::DrawContext(context);

}

//curve scaling
float z_scale = -50;
float r_scale = 3;

int currCurvePoint = 0;
float desiredCurveDuration = 10.;
void renderCurveFlyScene(GLFWwindow* window) {
	glClearColor(0.0f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	mat4 transformation;
	float time = glfwGetTime();

	glUseProgram(programCubeMap);
	mat4 viewProjectionMatrix = createPerspectiveMatrix() * createCameraMatrix() * translate(cameraPos);
	transformation = viewProjectionMatrix;
	glUniformMatrix4fv(glGetUniformLocation(programCubeMap, "transformation"), 1, GL_FALSE, (float*)&transformation);
	glBindTexture(GL_TEXTURE_CUBE_MAP, texture::cubemap);
	Core::DrawContext(cubeMapContex);
	glClear(GL_DEPTH_BUFFER_BIT);

	//SUN
	mat4 sunTransformation = scale(vec3(120));
	drawObjectColor(sphereContext, sunTransformation, vec3(0.9, 0.9, 0.2), programSun);

	//PLANET
	mat4 earthTransformation = rotate(float(time * 0.001), vec3(0, 1.0f, 0)) * translate(vec3(-2252.5, -2, 0)) * scale(vec3(25)) * rotate(-float(time * 0.125), vec3(0, 1.0f, 0));
	earthPosWor = vec3(earthTransformation * vec4(0, 0, 0, 1));
	earth_r = 25;
	drawObjectColorPBR(programEarthPBR, sphereContext, earthTransformation, rustediron2::albedo, rustediron2::normal, rustediron2::metallic, rustediron2::roughness, texture::clouds);


	//DEBUG
	//cameraPos = vec3(0, 0, 30.f /*+ 10.f * z_scale*/);
	//cameraDir = normalize(vec3(0, 0, -1.0));

	int step = 1;
	//in case of lag or debug deltaTime can be big and cause zero-devision
	if (desiredCurveDuration > deltaTime) {
		step = max(NUM_CURVE_POINTS / int((desiredCurveDuration / deltaTime)), 1);
	}
	//CURVE HANDLING
	vec3 curr = curve_points[currCurvePoint];
	// to prevent index out of range
	vec3 next = curve_points[min(currCurvePoint + step, NUM_CURVE_POINTS - 1)];
	vec3 spaceshipDir = - normalize(curr - next);
	vec3 spaceshipUp = normalize(vec3(curr.x, curr.y, 0));//dir to curve rotation axis
	vec3 spaceshipSide = normalize(cross(spaceshipUp, spaceshipDir));

	vec3 ship2planetVecW = earthPosWor - spaceshipPos;
	vec3 ship2planetNormalized = normalize(ship2planetVecW);
	float curveRotAngle = acos(dot(ship2planetNormalized, vec3(0, 0, -1)));
	vec3 curveRotAxis = normalize(cross(ship2planetNormalized, vec3(0, 0, 1)));

	//scale curve to desired shape
	curr.x *= r_scale;
	curr.y *= r_scale;
	curr.z *= z_scale;

	mat4 shipTransformation = mat4()
		* translate(spaceshipPos)
		* rotate(curveRotAngle, curveRotAxis)
		* translate(curr)
		* mat4({
			spaceshipSide.x,spaceshipSide.y,spaceshipSide.z,0,
			spaceshipUp.x,spaceshipUp.y,spaceshipUp.z ,0,
			spaceshipDir.x,spaceshipDir.y,spaceshipDir.z,0,
			0.,0.,0.,1.,
			})
		* scale(vec3(1));
	vec4 z_ax = normalize(vec4(0, 0, 1, 1) * shipTransformation);
	vec4 z_ax1 = normalize(vec4(0, 0, 1, 1) * rotate(curveRotAngle, curveRotAxis));
	vec4 spaceshipSide_ax = normalize(vec4(spaceshipSide, 1) * rotate(curveRotAngle, curveRotAxis));
	drawObjectColorPBR(programPBR, shipContext, shipTransformation, rustediron2::albedo, rustediron2::normal, rustediron2::metallic, rustediron2::roughness, texture::clouds);

	currCurvePoint = (currCurvePoint + step) % (NUM_CURVE_POINTS - 1);

	glUseProgram(0);
	glfwSwapBuffers(window);
}

void renderOnPlanetScene(GLFWwindow* window) {
	glClearColor(0.0f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	mat4 transformation;
	float time = glfwGetTime();

	glUseProgram(programCubeMap);
	mat4 viewProjectionMatrix = createPerspectiveMatrix() * createCameraMatrix() * translate(cameraPos);
	transformation = viewProjectionMatrix;
	glUniformMatrix4fv(glGetUniformLocation(programCubeMap, "transformation"), 1, GL_FALSE, (float*)&transformation);
	glBindTexture(GL_TEXTURE_CUBE_MAP, texture::cubemap);
	Core::DrawContext(cubeMapContex);
	glClear(GL_DEPTH_BUFFER_BIT);

	//PLANET
	mat4 earthTransformation = scale(vec3(PLANET_R));
	earthPosWor = vec3(earthTransformation * vec4(0, 0, 0, 1));
	earth_r = PLANET_R;
	drawObjectColorPBR(programEarthPBR, sphereContext, earthTransformation, rustediron2::albedo, rustediron2::normal, rustediron2::metallic, rustediron2::roughness, texture::clouds);

	//mat4 moonTransformation = translate(vec3(PLANET_R, 0, 0)) * scale(vec3(msf));
	//drawObjectColorPBR(programEarthPBR, sphereContext, moonTransformation, rustediron2::albedo, rustediron2::normal, rustediron2::metallic, rustediron2::roughness, texture::clouds);
	
	//SPACESHIP
	vec3 cameraSide = normalize(cross(spaceshipDir, vec3(0.f, 1.f, 0.f)));
	vec3 cameraUp = normalize(cross(cameraSide, spaceshipDir));
	mat4 shipTransformation = translate(spaceshipPos) * rotate(0.f, vec3(0, 1.0f, 0)) * mat4({
																														cameraSide.x,cameraSide.y,cameraSide.z,0,
																														cameraUp.x,cameraUp.y,cameraUp.z ,0,
																														spaceshipDir.x,spaceshipDir.y,spaceshipDir.z,0,
																														0.,0.,0.,1.,
		});
	//drawSpaceship(shipContext, shipTransformation, texture::ship, texture::scratches, texture::rust);
	drawObjectColorPBR(programPBR, shipContext, shipTransformation, rustediron2::albedo, rustediron2::normal, rustediron2::metallic, rustediron2::roughness, texture::clouds);

	//SPOTLIGHT
	mat4 spotTransformation = translate(spotPos) * rotate(0.f, vec3(0, 1.0f, 0)) * mat4({
																														cameraSide.x,cameraSide.y,cameraSide.z,0,
																														cameraUp.x,cameraUp.y,cameraUp.z ,0,
																														spotDir.x,spotDir.y,spotDir.z,0,
																														0.,0.,0.,1.,
		}
	) * scale(vec3(0.2));
	drawSpaceship(shipContext, spotTransformation, texture::ship, texture::scratches, texture::rust);


	glUseProgram(0);
	glfwSwapBuffers(window);
}

void renderInSpaceScene(GLFWwindow* window) {
	glClearColor(0.0f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	mat4 transformation;
	float time = glfwGetTime();

	glUseProgram(programCubeMap);
	mat4 viewProjectionMatrix = createPerspectiveMatrix() * createCameraMatrix() * translate(cameraPos);
	transformation = viewProjectionMatrix;
	glUniformMatrix4fv(glGetUniformLocation(programCubeMap, "transformation"), 1, GL_FALSE, (float*)&transformation);
	glBindTexture(GL_TEXTURE_CUBE_MAP, texture::cubemap);
	Core::DrawContext(cubeMapContex);
	glClear(GL_DEPTH_BUFFER_BIT);

	//SUN
	mat4 sunTransformation = scale(vec3(120));
	drawObjectColor(sphereContext, sunTransformation, vec3(0.9, 0.9, 0.2), programSun);

	//PLANET
	mat4 earthTransformation = rotate(float(time * 0.001), vec3(0, 1.0f, 0)) * translate(vec3(-2252.5, -2, 0)) * scale(vec3(25)) * rotate(-float(time * 0.125), vec3(0, 1.0f, 0));
	earthPosWor = vec3(earthTransformation * vec4(0, 0, 0, 1));
	earth_r = 25;
	drawObjectColorPBR(programEarthPBR, sphereContext, earthTransformation, rustediron2::albedo, rustediron2::normal, rustediron2::metallic, rustediron2::roughness, texture::clouds);

	mat4 moonTransformation = earthTransformation * rotate(time * 5, vec3(0, 1.0f, 0)) * translate(vec3(0, 0, 2)) * scale(vec3(0.5));

	//SPACESHIP
	vec3 cameraSide = normalize(cross(spaceshipDir, vec3(0.f, 1.f, 0.f)));
	vec3 cameraUp = normalize(cross(cameraSide, spaceshipDir));
	mat4 shipTransformation = translate(spaceshipPos) * rotate(0.f, vec3(0, 1.0f, 0)) * mat4({
																														cameraSide.x,cameraSide.y,cameraSide.z,0,
																														cameraUp.x,cameraUp.y,cameraUp.z ,0,
																														spaceshipDir.x,spaceshipDir.y,spaceshipDir.z,0,
																														0.,0.,0.,1.,
		});
	//drawSpaceship(shipContext, shipTransformation, texture::ship, texture::scratches, texture::rust);
	drawObjectColorPBR(programPBR, shipContext, shipTransformation, rustediron2::albedo, rustediron2::normal, rustediron2::metallic, rustediron2::roughness, texture::clouds);

	//SPOTLIGHT
	mat4 spotTransformation = translate(spotPos) * rotate(0.f, vec3(0, 1.0f, 0)) * mat4({
																														cameraSide.x,cameraSide.y,cameraSide.z,0,
																														cameraUp.x,cameraUp.y,cameraUp.z ,0,
																														spotDir.x,spotDir.y,spotDir.z,0,
																														0.,0.,0.,1.,
		}
	) * scale(vec3(0.2));
	drawSpaceship(shipContext, spotTransformation, texture::ship, texture::scratches, texture::rust);


	glUseProgram(0);
	glfwSwapBuffers(window);
}

void renderScene(GLFWwindow* window)
{
	switch (currSceneType)
	{
	case SceneType::IN_SPACE:
		renderInSpaceScene(window);
		break;
	case SceneType::ON_PLANET:
		renderOnPlanetScene(window);
		break;
	case SceneType::CURVE_FLY:
		renderCurveFlyScene(window);
		break;
	default:
		renderInSpaceScene(window);
		break;
	}
}
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	winWidth = width;
	winHeight = height;
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

//To handle key just once
// https://stackoverflow.com/questions/51873906/is-there-a-way-to-process-only-one-input-event-after-a-key-is-pressed-using-glfw
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	//DEBUG
	if (key == GLFW_KEY_TAB && action == GLFW_PRESS) {
		//switch between IN_SPACE and CURVE_FLY
		switch (currSceneType)
		{
		case SceneType::IN_SPACE:
			currSceneType = SceneType::CURVE_FLY;
			currCurvePoint = 0;
			break;
		case SceneType::CURVE_FLY:
			currSceneType = SceneType::IN_SPACE;
			break;
		default:
			break;
		}
		
		//switch to next scene type
		//currSceneType = SceneType((int(currSceneType) + 1) % int(SceneType::NUM_SCENE_TYPES));
		//spaceshipPos = vec3(1 * PLANET_R + SPACESHIP_H, 0, 0);
	}

	if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS) {
		if (S < maxS) {
			S *= 2;
		}
		printf("%f\n", S);
	}
	if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS) {
		if (S > minS) {
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

	//DEBUG
	if (key == GLFW_KEY_T && action == GLFW_PRESS) {
		z_scale += 1;
	}
	if (key == GLFW_KEY_G && action == GLFW_PRESS) {
		z_scale -= 1;
	}
}

bool checkIntersection(vec3 ray_dir) {
	vec3 p_center = earthPosWor;
	vec3 O = cameraPos;
	vec3 P = p_center;
	float dot_res = dot(P - O, ray_dir);

	//angle > 90
	if (dot_res < 0) {
		return false;
	}

	vec3 X = O + ray_dir * dot_res;
	float d = distance(P, X);
	
	//std::cout << "distance: " << d << std::endl;
	return d < earth_r;
}

vec3 screenCoord2WordVec(float xpos, double ypos) {
	// https://antongerdelan.net/opengl/raycasting.html
	mat4 perspectiveM = createPerspectiveMatrix();
	mat4 camM = createCameraMatrix();
	// Clip Space (Normalised Device Coordinates)
	vec2 ray_nds = vec2(2 * xpos / winWidth - 1, -(2 * ypos / winHeight - 1));
	vec4 ray_clip = vec4(ray_nds.x, ray_nds.y, -1.0, 1.0);
	// View space (Eye/Camera coordinates)
	// Only needed to un-project the x,y part, 
	//  set the z,w part to mean "forwards, and not a point". 
	vec4 ray_eye = inverse(perspectiveM) * ray_clip;
	ray_eye = vec4(ray_eye.x, ray_eye.y, -1.0, 0.0);
	// World Space
	vec3 ray_wor = vec3(inverse(camM) * ray_eye);
	ray_wor = normalize(ray_wor);

	//DEBUG
	//printf("%f,%f\n", ray_nds.x, ray_nds.y);
	//printVec3(ray_wor);

	return ray_wor;
}

void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
	addGlow = false;
	if (currSceneType == SceneType::IN_SPACE) {
		if (length(cameraPos - earthPosWor) < 1500) {
			addGlow = checkIntersection(screenCoord2WordVec(xpos, ypos));
		}
	}
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);
	
	if (button == GLFW_MOUSE_BUTTON_1 && action == GLFW_PRESS) {
		if (addGlow) {
			//switch to CURVE_FLY
			currSceneType = SceneType::CURVE_FLY;
			currCurvePoint = 0;
			//spaceshipPos = vec3(1 * PLANET_R + SPACESHIP_H, 0, 0);
			//spaceshipDir = normalize(- spaceshipPos);
			addGlow = false;
		}
	}
}

void generateSimple3dCurve() {
	// Curve from https://matplotlib.org/stable/gallery/mplot3d/lines3d.html
	constexpr float z_min = -2;
	constexpr float z_max = 2;
	constexpr float z_step = (z_max - z_min) / NUM_CURVE_POINTS;
	constexpr float rMax = z_max * z_max + 1;
	constexpr float theta_min = -4 * pi<float>();
	constexpr float theta_max = 4 * pi<float>();
	constexpr float theta_step = (theta_max - theta_min) / NUM_CURVE_POINTS;

	for (int i = 0; i < NUM_CURVE_POINTS; i++) {
		float z = z_min + z_step * i;
		float theta = theta_min + theta_step * i;
		float r = pow(z, 2) + 1;
		float x = r * sin(theta);
		float y = r * cos(theta);
		//scale z to [0, 1] and x, y to [-1, 1]
		z = (z - z_min) / (z_max - z_min);
		curve_points.push_back(vec3(x / rMax, y / rMax, z));
	}

}

void init(GLFWwindow* window)
{
	generateSimple3dCurve();

	// The window size is in screen coordinates, not pixels.
	//glfwGetWindowSize(window, &width, &height); 
	// In pixels
	glfwGetFramebufferSize(window, &winWidth, &winHeight);

	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, cursor_position_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);

	glEnable(GL_DEPTH_TEST);
	programSun = shaderLoader.CreateProgram("shaders/shader_5_sun.vert", "shaders/shader_5_sun.frag");
	programPBR = shaderLoader.CreateProgram("shaders/shader_PBR_test.vert", "shaders/shader_PBR_test.frag");
	programEarthPBR = shaderLoader.CreateProgram("shaders/shader_earthPBR_test.vert", "shaders/shader_PBR_test.frag");
	programSpaceship = shaderLoader.CreateProgram("shaders/shader_spaceship.vert", "shaders/shader_spaceship_test.frag");
	programCubeMap = shaderLoader.CreateProgram("shaders/shader_skybox.vert", "shaders/shader_skybox.frag");

	loadModelToContext("./models/sphere.obj", sphereContext);
	loadModelToContext("./models/spaceship.obj", shipContext);
	//loadModelToContext("./models/something.obj", hugeSphereContext);
	loadModelToContext("./models/cube.obj", cubeMapContex);


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

	glGenTextures(1, &texture::cubemap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, texture::cubemap);

	int w, h;
	unsigned char* data;
	std::vector<std::string> filepaths = {
		//lf-___-___-___-front-___
		"./textures/skybox/space_ft.png",
		"./textures/skybox/space_bk.png",
		"./textures/skybox/space_up.png",
		"./textures/skybox/space_dn.png",
		"./textures/skybox/space_rt.png",
		"./textures/skybox/space_lf.png",
	};
	for (unsigned int i = 0; i < 6; i++)
	{
		unsigned char* image = SOIL_load_image(filepaths[i].c_str(), &w, &h, 0, SOIL_LOAD_RGBA);
		glTexImage2D(
			GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
			0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image
		);

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	}

	glfwSetKeyCallback(window, key_callback);
}

void shutdown(GLFWwindow* window)
{
	shaderLoader.DeleteProgram(programPBR);
	shaderLoader.DeleteProgram(programEarthPBR);
	shaderLoader.DeleteProgram(programSun);
	shaderLoader.DeleteProgram(programSpaceship);
	shaderLoader.DeleteProgram(programCubeMap);
}

void onPlanetProcessInput(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		spaceshipPos = spaceshipPos + spaceshipDir * moveSpeed;

		printVec3(spaceshipPos);
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		spaceshipPos -= spaceshipDir * moveSpeed;

		printVec3(spaceshipPos);
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		spaceshipDir = vec3(eulerAngleY(angleSpeed) * vec4(spaceshipDir, 0));
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		spaceshipDir = vec3(eulerAngleY(-angleSpeed) * vec4(spaceshipDir, 0));
	}

	cameraPos = spaceshipPos - 1.5 * spaceshipDir + vec3(0, 0.5f, 0);
	cameraDir = spaceshipDir;

	//cameraDir = normalize(-cameraPos);

	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {
		exp_param += 10;
	}
	if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) {
		exp_param -= 10;
	}

	spotPos = spaceshipPos - vec3(normalize(spaceshipDir).x, -1.f, normalize(spaceshipDir).z);
	spotDir = spotLightOn * (spaceshipDir + vec3(0, -0.5f, 0));
}

//obsluga wejscia
void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}

	float time = glfwGetTime();
	deltaTime = time - lastFrameTime;
	lastFrameTime = time;
	float fps = 1 / deltaTime;

	angleSpeed = R / fps;
	moveSpeed = S / fps;

	if (currSceneType == SceneType::ON_PLANET) {
		onPlanetProcessInput(window);
		return;
	}

	if (currSceneType == SceneType::CURVE_FLY) {
		return;
	}

	vec3 spaceshipSide = normalize(cross(spaceshipDir, vec3(0.f, 1.f, 0.f)));

	
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		spaceshipPos = spaceshipPos + spaceshipDir * moveSpeed;

		printVec3(spaceshipPos);
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		spaceshipPos -= spaceshipDir * moveSpeed;

		printVec3(spaceshipPos);
	}
	if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS) {
		spaceshipPos += spaceshipSide * moveSpeed;
	}
	if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS) {
		spaceshipPos -= spaceshipSide * moveSpeed;
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		spaceshipDir = vec3(eulerAngleY(angleSpeed) * vec4(spaceshipDir, 0));
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		spaceshipDir = vec3(eulerAngleY(-angleSpeed) * vec4(spaceshipDir, 0));
	}
	
	cameraPos = spaceshipPos - 1.5 * spaceshipDir + vec3(0, 0.5f, 0);
	cameraDir = spaceshipDir;

	//cameraDir = normalize(-cameraPos);

	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {
		exp_param += 10;
	}
	if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) {
		exp_param -= 10;
	}

	spotPos = spaceshipPos - vec3(normalize(spaceshipDir).x, -1.f, normalize(spaceshipDir).z);
	spotDir = spotLightOn * (spaceshipDir + vec3(0, -0.5f, 0));
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