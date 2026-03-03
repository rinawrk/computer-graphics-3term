#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <stdio.h>

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

#include "Shader.h"
#include "Model.h"

static const unsigned int SCR_WIDTH = 1024;
static const unsigned int SCR_HEIGHT = 768;

static void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

int main(void)
{
    // 1) GLFW
    if (!glfwInit()) {
        fprintf(stderr, "ERROR: could not start GLFW\n");
        return 1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Lab 5 - Assimp Model", NULL, NULL);
    if (!window) {
        fprintf(stderr, "ERROR: could not open window\n");
        glfwTerminate();
        return 1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // 2) GLEW
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "ERROR: could not start GLEW\n");
        glfwTerminate();
        return 1;
    }

    // 3) Depth test для 3D
    glEnable(GL_DEPTH_TEST);

    // 4) Шейдер для модели
    Shader shader("shaders/model.vert", "shaders/model.frag");
    if (shader.ID == 0) {
        fprintf(stderr, "ERROR: shader program not created\n");
        glfwTerminate();
        return 1;
    }

    // 5) Загружаем модель (куб-заглушка в assets)
    Model model("../assets/cube_zaglushka/Cube.obj");

    // 6) Статичная камера (пока без управления)
    glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 6.0f);
    glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

    while (!glfwWindowShouldClose(window))
    {
        // Закрытие по Esc
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

        glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader.use();

        // Матрицы
        glm::mat4 projection = glm::perspective(
            glm::radians(45.0f),
            (float)SCR_WIDTH / (float)SCR_HEIGHT,
            0.1f, 100.0f
        );

        glm::mat4 view = glm::lookAt(cameraPos, cameraTarget, cameraUp);

        glm::mat4 modelMat = glm::mat4(1.0f); // можно будет двигать/масштабировать модель

        // Передаём uniforms
        glUniformMatrix4fv(glGetUniformLocation(shader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(glGetUniformLocation(shader.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(shader.ID, "model"), 1, GL_FALSE, glm::value_ptr(modelMat));

        // Цвет плоской заливки (lightColor)
        glm::vec3 lightColor(0.85f, 0.85f, 0.85f);
        glUniform3f(glGetUniformLocation(shader.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z);

        // Рисуем модель
        model.Draw();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}