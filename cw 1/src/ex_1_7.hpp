#pragma once
#include "glew.h"
#include <GLFW/glfw3.h>
#include "glm.hpp"
#include "ext.hpp"
#include <vector>

#include "Shader_Loader.h"
#include "Render_Utils.h"

GLuint program; // Shader ID

GLuint quadVAO;

Core::Shader_Loader shaderLoader;

std::vector<glm::vec3> quadsPositions;
glm::vec3 quadPos(0.0, 0.0, 0.0);
float angle = 0.0f;

void renderScene(GLFWwindow* window)
{
    quadsPositions.push_back(quadPos);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.0f, 0.3f, 0.3f, 1.0f);

	float time = glfwGetTime();

    glUseProgram(program);


    glm::mat4 transformation;

    transformation = glm::translate(quadPos);
    transformation = transformation * glm::translate(quadsPositions[0]);
    //transformation = transformation * glm::translate(glm::vec3(0, sin(time)/2, 0));
    //transformation = transformation * glm::scale(glm::vec3(sin(time)/2+1, sin(time)/2+1,0));
    //transformation = transformation * glm::rotate(time, glm::vec3(0,0,1));
    transformation = transformation * glm::rotate(angle, glm::vec3(0,0,1));
    


    glUniformMatrix4fv(glGetUniformLocation(program, "transformation"), 1, GL_FALSE, (float*)&transformation);
    Core::drawVAOIndexed(quadVAO, 6);

    glUseProgram(0);

    glfwSwapBuffers(window);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}
void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
    //quadPos[0] = 2 * xpos / 500 - 1;
    //quadPos[1] = -(2 * ypos / 500 - 1);
}
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    printf("%d,%d\n", button, action);
    if ((button == 0) && (action == 0)) {
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
        printf("%f,%f\n", xpos, ypos);
        quadPos[0] = 2 * xpos / 500 - 1;
        quadPos[1] = -(2 * ypos / 500 - 1);
    }
}



void init(GLFWwindow* window) {
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetInputMode(window, GLFW_STICKY_MOUSE_BUTTONS, GLFW_TRUE);
    program = shaderLoader.CreateProgram("shaders/shader_1_2.vert", "shaders/shader_1_2.frag");

    float points[] = {
        -0.1,-0.1,0.0,1.0,
         0.1,-0.1,0.0,1.0,
         0.1, 0.1,0.0,1.0,
        -0.1, 0.1,0.0,1.0,
    };
    unsigned int indices[] = {
        0,1,3,
        1,2,3,
    };
    quadVAO = Core::initVAOIndexed(points, indices, 4, 4, 6);

    //Przekopiuj kod do ladowania z poprzedniego zadania
}

void shutdown(GLFWwindow* window)
{
    shaderLoader.DeleteProgram(program);
}

//obsluga wejscia
void processInput(GLFWwindow* window)
{
    float time = glfwGetTime();
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
        quadPos[1] += 0.1;
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
        quadPos[1] -= 0.1;
    }
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
        quadPos[0] += 0.1;
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
       quadPos[0] -= 0.1;
    }
    if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS) {
        angle -= 0.1;
    }
    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) {
        angle += 0.1;
    }
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