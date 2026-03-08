// Лабораторная работа №4: Движение камеры, использование матриц. Работа с библиотекой glm.

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <stdio.h>  // fprintf
#include <math.h>   // sinf/cosf

// GLM: векторы/матрицы и функции преобразований
#include <glm.hpp>                  // glm/glm.hpp
#include <gtc/matrix_transform.hpp> // glm/gtc/matrix_transform.hpp
#include <gtc/type_ptr.hpp>         // glm/gtc/type_ptr.hpp

#include "Shader.h"

static const unsigned int SCR_WIDTH = 1024;
static const unsigned int SCR_HEIGHT = 768;

// ====================== Камера (глобальные параметры) ======================

// Позиция камеры в мире
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);

// Направление, куда камера смотрит (обновляется мышью)
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);

// Вектор "вверх" для камеры
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

/* Первичный вариант из методички: фиксированная цель камеры
glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
тогда view можно считать так:
glm::mat4 view = glm::lookAt(cameraPos, cameraTarget, cameraUp); */

// ========================== Мышь: углы yaw/pitch ===========================

float yaw = -90.0f;
float pitch = 0.0f; // камера смотрит ровно, без наклона вверх/вниз

float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;

bool  firstMouse = true;
float sensitivity = 0.1f; // чувствительность мыши

// =========== Время кадра (для одинаковой скорости на разном FPS) ============

float deltaTime = 0.0f; // сколько секунд прошло с прошлого кадра
float lastFrame = 0.0f; // время прошлого кадра

/* Первичный вариант из методички: без deltaTime
В этом варианте скорость зависит от FPS - движение на фиксированный шаг за кадр
const float cameraSpeedFixed = 0.05f; */

// ============================================================================
// Обработка клавиатуры: WASD перемещает камеру
// ============================================================================

void processInput(GLFWwindow* window)
{
    // Скорость в "условных единицах в секунду" (скорость зависит от времени кадра)
    const float cameraSpeed = 2.5f * deltaTime;

    // Esc — закрыть окно
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    // W/S — вперёд/назад
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPos += cameraSpeed * cameraFront;

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPos -= cameraSpeed * cameraFront;

    // A/D — влево/вправо
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
}

// ============================================================================
// Callback мыши: обновляет yaw/pitch и пересчитывает cameraFront
// ============================================================================
void mouse_callback(GLFWwindow* /*window*/, double xposIn, double yposIn)
{
    float xpos = (float)xposIn;
    float ypos = (float)yposIn;

    // Первый вход мыши — чтобы убрать резкий скачок
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    // Смещение курсора с прошлого кадра
    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // вверх = +

    lastX = xpos;
    lastY = ypos;

    // Чувствительность мыши
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    // Обновляем углы
    yaw += xoffset;
    pitch += yoffset;

    // Ограничиваем pitch, чтобы не переворачивалась камера
    if (pitch > 89.0f)  pitch = 89.0f;
    if (pitch < -89.0f) pitch = -89.0f;

    // Пересчитываем направление взгляда cameraFront из yaw/pitch
    glm::vec3 front;
    front.x = cosf(glm::radians(yaw)) * cosf(glm::radians(pitch));
    front.y = sinf(glm::radians(pitch));
    front.z = sinf(glm::radians(yaw)) * cosf(glm::radians(pitch));

    cameraFront = glm::normalize(front);
}

// ============================================================================
// main(): создание окна, настройка OpenGL, рендер-цикл
// ============================================================================

int main(void)
{
    // 1) Инициализация GLFW
    if (!glfwInit()) {
        fprintf(stderr, "ERROR: could not start GLFW\n");
        return 1;
    }

    // 2) Настройки контекста OpenGL
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // 3) Создание окна GLFW
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Lab 4 - Cube", NULL, NULL);
    if (!window) {
        fprintf(stderr, "ERROR: could not open window\n");
        glfwTerminate();
        return 1;
    }
    // Делаем OpenGL-контекст этого окна текущим
    glfwMakeContextCurrent(window);

    // 4) Фиксируем курсор мыши внутри окна для удобного управления камерой
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    // Подписываемся на движение мыши
    glfwSetCursorPosCallback(window, mouse_callback);

    // 5) Инициализация GLEW
    glewExperimental = GL_TRUE; // Разрешаем GLEW использовать современные функции OpenGL
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "ERROR: could not start GLEW\n");
        glfwTerminate();
        return 1;
    }

    // 6) Включаем depth test для корректного отображения ближних и дальних граней
    glEnable(GL_DEPTH_TEST);

    // Режим отрисовки:
    // GL_FILL — обычный (грани закрашены)
    // GL_LINE — каркас (видны только рёбра)
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // 7) Вершины куба (36 вершин = 12 треугольников)
    float vertices[] = {
        // back
        -0.5f,-0.5f,-0.5f,  0.5f,-0.5f,-0.5f,  0.5f, 0.5f,-0.5f,
         0.5f, 0.5f,-0.5f, -0.5f, 0.5f,-0.5f, -0.5f,-0.5f,-0.5f,
        // front
        -0.5f,-0.5f, 0.5f,  0.5f,-0.5f, 0.5f,  0.5f, 0.5f, 0.5f,
         0.5f, 0.5f, 0.5f, -0.5f, 0.5f, 0.5f, -0.5f,-0.5f, 0.5f,
        // left
        -0.5f, 0.5f, 0.5f, -0.5f, 0.5f,-0.5f, -0.5f,-0.5f,-0.5f,
        -0.5f,-0.5f,-0.5f, -0.5f,-0.5f, 0.5f, -0.5f, 0.5f, 0.5f,
        // right
         0.5f, 0.5f, 0.5f,  0.5f, 0.5f,-0.5f,  0.5f,-0.5f,-0.5f,
         0.5f,-0.5f,-0.5f,  0.5f,-0.5f, 0.5f,  0.5f, 0.5f, 0.5f,
        // bottom
        -0.5f,-0.5f,-0.5f,  0.5f,-0.5f,-0.5f,  0.5f,-0.5f, 0.5f,
         0.5f,-0.5f, 0.5f, -0.5f,-0.5f, 0.5f, -0.5f,-0.5f,-0.5f,
        // top
        -0.5f, 0.5f,-0.5f,  0.5f, 0.5f,-0.5f,  0.5f, 0.5f, 0.5f,
         0.5f, 0.5f, 0.5f, -0.5f, 0.5f, 0.5f, -0.5f, 0.5f,-0.5f
    };

    // 8) VAO/VBO: отправляем вершины на GPU
    GLuint VAO = 0, VBO = 0;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // 9) Загрузка шейдеров
    Shader shader("shaders/camera.vert", "shaders/camera.frag");
    if (shader.ID == 0) {
        fprintf(stderr, "ERROR: shader program not created\n");
        glfwTerminate();
        return 1;
    }

    // 10) Основной цикл
    while (!glfwWindowShouldClose(window))
    {
        // Время кадра (для движения с deltaTime)
        float currentFrame = (float)glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // Обработка клавиатуры
        processInput(window);

        // Фон (цвет - графит)
        glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader.use();

        /*
        Камера LookAt (первичный вариант из методички)
        glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f); // eye
        glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f); // center
        glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f); // up

        Камера смотрит на фиксированную точку
        glm::mat4 view = glm::lookAt(cameraPos, cameraTarget, cameraUp);
        */

        // Камера смотрит вперёд по направлению cameraFront (WASD + мышь)
        glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

        // PROJECTION: перспектива
        glm::mat4 projection = glm::perspective(
            glm::radians(45.0f),
            (float)SCR_WIDTH / (float)SCR_HEIGHT,
            0.1f, 100.0f
        );

        // MODEL: базовая матрица объекта
        glm::mat4 model = glm::mat4(1.0f);

        // TRANSFORM: дополнительное преобразование (фиксированный поворот)
        glm::mat4 transform = glm::mat4(1.0f);
        transform = glm::rotate(transform, glm::radians(30.0f), glm::vec3(0.0f, 1.0f, 0.0f));

        // Передаём матрицы в шейдер
        glUniformMatrix4fv(glGetUniformLocation(shader.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(shader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(glGetUniformLocation(shader.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(glGetUniformLocation(shader.ID, "transform"), 1, GL_FALSE, glm::value_ptr(transform));

        // Отрисовка
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // 11) Очистка ресурсов
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

    // 12) Завершение работы GLFW
    glfwTerminate();
    return 0;
}
