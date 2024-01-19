#pragma once
#include "glew.h"
#include <GLFW/glfw3.h>
#include "glm.hpp"

#include "Shader_Loader.h"
#include "Render_Utils.h"

GLuint program; // Shader ID

GLuint quadVAO;

Core::Shader_Loader shaderLoader;


void renderScene(GLFWwindow* window)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.0f, 0.3f, 0.3f, 1.0f);

	// Zmienna "time" przechowuje czas ktory uplynal od uruchomienia aplikacji
	float time = glfwGetTime();

    glUseProgram(program);
    
    // ZADANIE: Wyswietl czworokat, ktory jednoczesnie przesuwa sie i obraca.
    // 
    // Uzyj kodu do translacji i rotacji czworokatu z poprzednich zadan
    // 
    glm::mat4 rotation =
    { cos(time), -sin(time),0,0,
      sin(time),cos(time),0,0,
      0,0,1,0,
      0,0,0,1 };

    glm::mat4 translation =
    { 1,0,0,0,
      0,1,0,sin(time),
      0,0,1,0,
      0,0,0,1 };
    // 
    // Stworz taka macierz transformacji, ktora powoduje ze czworokat przesuwa sie w gore i w dol ekranu, jednoczesnie obracajac sie.
    glm::mat4 together = rotation * translation;
    // Wyslij stworzona macierz do GPU za pomoca funkcji glUniformMatrix4fv zamiast macierzy "translation" i "rotation" z poprzednich zadan
    together = glm::transpose(together);
    glUniformMatrix4fv(glGetUniformLocation(program, "transformation"), 1, GL_FALSE, (float*)&together);
    // Uzyj kodu z poprzednich cwiczen do narysowania czworokata
    Core::drawVAOIndexed(quadVAO, 6);

    glUseProgram(0);

    glfwSwapBuffers(window);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void init(GLFWwindow* window) {
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    program = shaderLoader.CreateProgram("shaders/shader_1_2.vert", "shaders/shader_1_2.frag");

    //Przekopiuj kod do ladowania z poprzedniego zadania
    float quadPoints[] = {
        -0.5, -0.5, 0, 1,
        0.5, -0.5, 0, 1,
        0.5, 0.5, 0, 1,
        -0.5, 0.5, 0, 1,
    };
    unsigned int quadIndexed[] = {
        0, 1, 3,
        1, 2, 3
    };
    quadVAO = Core::initVAOIndexed(quadPoints, quadIndexed, 4, 4, 6);
}

void shutdown(GLFWwindow* window)
{
    shaderLoader.DeleteProgram(program);
}


//obsluga wejscia
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
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