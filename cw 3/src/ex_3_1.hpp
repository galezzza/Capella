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
#include <assimp\Importer.hpp>

#define M_PI 3.1415926535897932384626433832795

GLuint program, program_box;
Core::Shader_Loader shaderLoader;

Core::RenderContext shipContext;
Core::RenderContext sphereContext;

glm::vec3 cameraPos = glm::vec3(0.f, 0.f, -1.65f);
glm::vec3 cameraDir = glm::vec3(0.f, 0.f, 1.65f);


glm::vec3 p = glm::vec3(0.f, 0.f, 0.0f);
GLuint VAO,VBO;

float aspectRatio = 1.f;

float box_1[sizeof(box)];
float box_2[sizeof(box)];

float S(float fov) {
	return 1 / tan(fov * M_PI / 360);
}

//const aiScene* scene = import.ReadFile("./models/spaceship.obj", aiProcess_TriangulateaiProcess_Triangulate | aiProcess_CalcTangentSpace);

glm::mat4 createCameraMatrix(){

	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// ! Macierz translation jest definiowana wierszowo dla poprawy czytelnosci. OpenGL i GLM domyslnie stosuje macierze kolumnowe, dlatego musimy ja transponowac !
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	glm::vec3 cameraSide;
	glm::vec3 cameraUp;
	cameraSide = glm::normalize(cross(cameraDir, glm::vec3(0.f, 1.f, 0.f)));
	cameraUp = glm::normalize(cross(cameraSide, cameraDir));
	
	

	glm::mat4 cameraRotationMatrix = glm::mat4({
		cameraSide[0],cameraSide[1],cameraSide[2],0.,
		cameraUp[0],cameraUp[1],cameraUp[2],0.,
		-cameraDir[0],-cameraDir[1],-cameraDir[2],0.,
		0.,0.,0.,1.,
		});
	cameraRotationMatrix = glm::transpose(cameraRotationMatrix);

	glm::mat4 la = glm::translate(-1*cameraPos);
	cameraRotationMatrix = cameraRotationMatrix * la;
	//printf("%f %f %f\n", cameraPos.x, cameraPos.y, cameraPos.z);

	

	glm::mat4 cameraMatrix= cameraRotationMatrix;

	return cameraMatrix;
}

glm::mat4 createPerspectiveMatrix(float fov, float aspectRatio){


	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// ! Macierz translation jest definiowana wierszowo dla poprawy czytelnosci. OpenGL i GLM domyslnie stosuje macierze kolumnowe, dlatego musimy ja transponowac !
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	float n = 0.1;
	float f = 100.7;
	//float fov = 90;


	glm::mat4 perspectiveMatrix;
	
	if (aspectRatio < 1) {
		perspectiveMatrix = glm::mat4({
			S(fov) * aspectRatio,0.,0.,0.,
			0.,S(fov),0.,0.,
			0.,0.,(n + f) / (n - f),(2 * n * f) / (n - f),
			0.,0.,-1.,0.,
		});
	}
	else {
		perspectiveMatrix = glm::mat4({
			S(fov),0.,0.,0.,
			0.,S(fov) / aspectRatio,0.,0.,
			0.,0.,(n + f) / (n - f),(2 * n * f) / (n - f),
			0.,0.,-1.,0.,
		});
	}

	perspectiveMatrix=glm::transpose(perspectiveMatrix);

	return perspectiveMatrix;
}
void renderScene(GLFWwindow* window)
{
	glClearColor(0.0f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::mat4 transformation;

	glUseProgram(program_box);



	glBindVertexArray(VAO);
	transformation = createPerspectiveMatrix(90.f, aspectRatio)	* createCameraMatrix();
	glUniformMatrix4fv(glGetUniformLocation(program_box, "transformation"), 1, GL_FALSE, (float*)&transformation);
	glDrawArrays(GL_TRIANGLES, 0, 108);
	glBindVertexArray(0);

	glUseProgram(0);
	glfwSwapBuffers(window);
}
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{	
	aspectRatio = float(height) / width;

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
}

void init(GLFWwindow* window)
{
	for (int i = 0; i < sizeof(box)/sizeof(float); i++) {
		box_1[i] = box[i];
		box_2[i] = box[i];
		if (i % 8 == 0) {
			box_1[i] += 1;
			box_2[i] -= 1;
		}
	}

	

	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	glEnable(GL_DEPTH_TEST);
	program = shaderLoader.CreateProgram("shaders/shader_3_1.vert", "shaders/shader_3_1.frag");
	program_box = shaderLoader.CreateProgram("shaders/shader_2_1.vert", "shaders/shader_2_1.frag");

	glGenVertexArrays(1, &VAO);
	// 2. Powiąż stworzone VAO za pomocą funkcji glBindVertexArray
	glBindVertexArray(VAO);
	// II Zainicjalizuj VBO
	// 3. Stwórz VertexBufferObject do przechowywania pozycji wierzchołków za pomocą funkcji glGenBuffers
	glGenBuffers(1, &VBO);
	// 4. Powiąż stworzone VBO za pomocą funkcji glBindBuffer(GLenum target, GLuint )
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	// 5. Zaalokuj w GPU miejsce na tablice box i zaalakokuj ją za pomocą glBufferData
	glBufferData(GL_ARRAY_BUFFER, sizeof(box)*3, box, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(box), sizeof(box), box_1);
	glBufferSubData(GL_ARRAY_BUFFER, 2*sizeof(box), sizeof(box), box_2);
	// II Stwórz definicję danych w buforze
	// 6. Aktywuj atrybut powiązany z pozycją wierchołków za pomocą glEnableVertexAttribArray(GLuint index). Indeks jest zapisany w shaderze wierzchołków w 3. lini. Można odpytać shader o indeks za pomocą funkcji glGetAttribLocation(GL uint program, const GLchar *name)
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(4 * sizeof(float)));

}

void shutdown(GLFWwindow* window)
{
	shaderLoader.DeleteProgram(program);
}

//obsluga wejscia
void processInput(GLFWwindow* window)
{
	glm::vec3 cameraSideY = glm::normalize(glm::cross(cameraDir, glm::vec3(0.f,1.f,0.f)));
	glm::vec3 cameraSideX = glm::normalize(glm::cross(cameraDir, glm::vec3(cameraDir[2], 0.f, -cameraDir[0])));
	float angleSpeed = 0.05f;
	float moveSpeed = 0.05f;
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		cameraPos += cameraDir * moveSpeed;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		cameraPos -= cameraDir * moveSpeed;
	if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
		cameraPos -= cameraSideY * moveSpeed;
	if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS)
		cameraPos += cameraSideY * moveSpeed;
	//if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		//cameraDir = glm::vec3(glm::eulerAngleY(angleSpeed) * glm::vec4(cameraDir, 0));
	//if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		//cameraDir = glm::vec3(glm::eulerAngleY(-angleSpeed) * glm::vec4(cameraDir, 0));
	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
		cameraPos += cameraSideX * moveSpeed;
		//printf("%f %f %f\n", cameraSideX[0], cameraSideX[1], cameraSideX[2]);
	if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS)
		cameraPos -= cameraSideX * moveSpeed;
		//printf("%f %f %f\n", cameraSideX[0], cameraSideX[1], cameraSideX[2]);
	cameraDir = glm::normalize(glm::vec3(-cameraPos[0], -cameraPos[1], -cameraPos[2]));
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