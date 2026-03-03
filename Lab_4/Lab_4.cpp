#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <stdio.h> // fprintf
#include <math.h>  // sinf/cosf

#include "Shader.h"

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

static const unsigned int SCR_WIDTH = 1024;
static const unsigned int SCR_HEIGHT = 768;

// Параметры камеры (глобально)
// cameraPos — позиция камеры
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);

// cameraFront — направление взгляда
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);

// cameraUp — вверх для камеры
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

// Мышь (для поворота камеры)
float yaw = -90.0f;
float pitch = 0.0f;

float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;

bool firstMouse = true;

float sensitivity = 0.1f; // чувствительность мыши

// deltaTime
float deltaTime = 0.0f;  // сколько секунд прошло с прошлого кадра
float lastFrame = 0.0f;  // время прошлого кадра

// функция ввода
void processInput(GLFWwindow* window)
{
    // const float cameraSpeed = 0.05f; // шаг перемещения - без deltaTime (движение на фиксированный шаг за кадр, если FPS низкий, шагов меньше - камера еле ползёт)
    const float cameraSpeed = 2.5f * deltaTime; // движение на фиксированное расстояние за секунду

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

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = (float)xposIn;
    float ypos = (float)yposIn;

    // Первый заход мыши: чтобы не было резкого рывка
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    // Смещения мыши
    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // вверх = +
    lastX = xpos;
    lastY = ypos;

    // Чувствительность
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    // Обновляем углы
    yaw += xoffset;
    pitch += yoffset;

    // Ограничиваем pitch, чтобы камера не переворачивалась
    if (pitch > 89.0f)  pitch = 89.0f;
    if (pitch < -89.0f) pitch = -89.0f;

    // Пересчитываем направление взгляда cameraFront из yaw/pitch
    glm::vec3 front;
    front.x = cosf(glm::radians(yaw)) * cosf(glm::radians(pitch));
    front.y = sinf(glm::radians(pitch));
    front.z = sinf(glm::radians(yaw)) * cosf(glm::radians(pitch));
    cameraFront = glm::normalize(front);
}

int main(void)

{
    if (!glfwInit()) {
        fprintf(stderr, "ERROR: could not start GLFW\n");
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Lab 4 - Cube", NULL, NULL);
    if (!window) {
        fprintf(stderr, "ERROR: could not open window\n");
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    // Захватываем курсор мыши внутри окна (иначе он будет улетать)
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Подписываемся на движение мыши
    glfwSetCursorPosCallback(window, mouse_callback);

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "ERROR: could not start GLEW\n");
        glfwTerminate();
        return -1;
    }

    // В 3D включаем тест глубины
    glEnable(GL_DEPTH_TEST);

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    // добавила, чтобы можно было посмотреть рёбра - GL_LINE или закрашенные грани - GL_FILL

    // Куб (36 вершин = 12 треугольников)
    float vertices[] = {
        // 
        -0.5f,-0.5f,-0.5f,  0.5f,-0.5f,-0.5f,  0.5f, 0.5f,-0.5f,
         0.5f, 0.5f,-0.5f, -0.5f, 0.5f,-0.5f, -0.5f,-0.5f,-0.5f,
        // 
        -0.5f,-0.5f, 0.5f,  0.5f,-0.5f, 0.5f,  0.5f, 0.5f, 0.5f,
         0.5f, 0.5f, 0.5f, -0.5f, 0.5f, 0.5f, -0.5f,-0.5f, 0.5f,
        // 
        -0.5f, 0.5f, 0.5f, -0.5f, 0.5f,-0.5f, -0.5f,-0.5f,-0.5f,
        -0.5f,-0.5f,-0.5f, -0.5f,-0.5f, 0.5f, -0.5f, 0.5f, 0.5f,
        // 
         0.5f, 0.5f, 0.5f,  0.5f, 0.5f,-0.5f,  0.5f,-0.5f,-0.5f,
         0.5f,-0.5f,-0.5f,  0.5f,-0.5f, 0.5f,  0.5f, 0.5f, 0.5f,
        // 
        -0.5f,-0.5f,-0.5f,  0.5f,-0.5f,-0.5f,  0.5f,-0.5f, 0.5f,
         0.5f,-0.5f, 0.5f, -0.5f,-0.5f, 0.5f, -0.5f,-0.5f,-0.5f,
        // 
        -0.5f, 0.5f,-0.5f,  0.5f, 0.5f,-0.5f,  0.5f, 0.5f, 0.5f,
         0.5f, 0.5f, 0.5f, -0.5f, 0.5f, 0.5f, -0.5f, 0.5f,-0.5f
    };

    // VAO/VBO
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

    // Шейдеры из файлов
    Shader shader("shaders/camera.vert", "shaders/camera.frag");
    if (shader.ID == 0) {
        fprintf(stderr, "ERROR: shader program not created\n");
        glfwTerminate();
        return -1;
    }

    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = (float)glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        glClearColor(0.2f, 0.2f, 0.2f, 1.0f); // цвет фона - графит
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader.use();

        /*
        Камера LookAt (как сначала в методичке)
        glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f); // eye
        glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f); // center
        glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f); // up

        Вариант A: камера смотрит на фиксированную точку
        glm::mat4 view = glm::lookAt(cameraPos, cameraTarget, cameraUp);
        */

        // Вариант B (актуальный для WASD/мыши): камера смотрит "вперёд"
        glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

        // ПРОЕКЦИЯ (projection): перспектива
        glm::mat4 projection = glm::perspective(
            glm::radians(45.0f),
            (float)SCR_WIDTH / (float)SCR_HEIGHT,
            0.1f, 100.0f
        );

        // MODEL: положение объекта
        glm::mat4 model = glm::mat4(1.0f);

        // TRANSFORM: дополнительное преобразование (повернём, чтобы видно было 3D)
        glm::mat4 transform = glm::mat4(1.0f);
        transform = glm::rotate(transform, glm::radians(30.0f), glm::vec3(0.0f, 1.0f, 0.0f));

        // Отправляем матрицы в шейдер
        glUniformMatrix4fv(glGetUniformLocation(shader.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(shader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(glGetUniformLocation(shader.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(glGetUniformLocation(shader.ID, "transform"), 1, GL_FALSE, glm::value_ptr(transform));

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

    glfwTerminate();
    return 0;
}
