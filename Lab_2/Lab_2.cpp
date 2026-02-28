// Лабораторная работа №2: Современный OpenGL(4.6), шейдеры, VBO/VAO/EBO, пульсация цвета.
// Вариант 10: квадрат, базовый цвет фигуры (1.0, 0.8, 0.5), фон (0.2, 1.0, 1.0).

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <stdio.h>      // для printf / fprintf
#include <math.h>       // для sinf

#include "Shader.h"     // подключение файла Shader.h

int main(void)
{
    // 1) Инициализация GLFW
    if (!glfwInit()) {
        fprintf(stderr, "ERROR: could not start GLFW3.\n");
        return 1;
    }

    // 2) Установка версии OpenGL 4.6
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // 3) Создание окна и OpenGL контекста
    GLFWwindow* window = glfwCreateWindow(512, 512, "Lab 2 - Variant 10", NULL, NULL);
    if (!window) {
        fprintf(stderr, "ERROR: could not open window with GLFW3.\n");
        glfwTerminate();
        return 1;
    }

    // Делаем созданный контекст текущим
    glfwMakeContextCurrent(window);

    // 4) Инициализация GLEW
    glewExperimental = GL_TRUE; // режим GLEW для поддержки современных функций
    GLenum ret = glewInit();
    if (GLEW_OK != ret) {
        fprintf(stderr, "Error: %s\n", glewGetErrorString(ret));
        glfwTerminate();
        return 1;
    }

    // 5) Данные фигуры
    // Квадрат = 4 вершины, каждая вершина = (x, y, z)
    // Кординаты вершин:
    float vertices[] = {
        // x,    y,    z
        -0.5f,  0.5f,  0.0f,  // левая верхняя = 0
         0.5f,  0.5f,  0.0f,  // правая верхняя = 1
         0.5f, -0.5f,  0.0f,  // правая нижняя = 2
        -0.5f, -0.5f,  0.0f   // левая нижняя = 3
    };

    // 6) Индексы для координат вершин: 
    // квадрат = 2 треугольника
    // треугольники (0,1,2) и (2,3,0) — диагональ 0-2
    unsigned int indices[] = {
        0, 1, 2,
        2, 3, 0
    };

    // 7) Создание VAO/VBO/EBO
    
    // VAO: "память" о настройке атрибутов вершин
    // VBO: буфер с вершинами на GPU
    // EBO: буфер с индексами на GPU

    // Генерация индентификаторов
    GLuint VAO = 0, VBO = 0, EBO = 0;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    // Привязываем VAO: все последующие настройки атрибутов будут сохранены в этом VAO
    glBindVertexArray(VAO);

    // VBO: загружаем массив вершин в память видеокарты
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // EBO: загружаем индексы треугольников
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Настройка формата вершинных данных (как интерпретировать VBO)
    glVertexAttribPointer(
        0,                      // индекс атрибута (location = 0)
        3,                      // каждая вершина состоит из 3 компонентов (x, y, z)
        GL_FLOAT,               // тип данных - вещественные числа
        GL_FALSE,               // нормализация отключена
        3 * sizeof(float),      // шаг между вершинами
        (void*)0                // смещение в буфере 0 - данные начинаются с начала буфера
    );
    glEnableVertexAttribArray(0);      // включаем использование атрибута 0

    glBindBuffer(GL_ARRAY_BUFFER, 0);  // отвязали VBO
    glBindVertexArray(0);              // отвязали VAO

    // 8) Задание 2: загружаем шейдеры из файлов
    Shader shader("shaders/basic.vert", "shaders/basic.frag");
    if (shader.ID == 0) {
        fprintf(stderr, "ERROR: shader program not created.\n");
        glfwTerminate();
        return 1;
    }

    // 9) Основной цикл рендера
    while (!glfwWindowShouldClose(window)) {

        // Установка цвета фона кадра (вариант 10: (0.2,1.0,1.0))
        glClearColor(0.2f, 1.0f, 1.0f, 1.0f);
        // Очистка кадра
        glClear(GL_COLOR_BUFFER_BIT);

        // Задание 1: зависимость цвета от времени
        // Делаем пульсацию яркости, k - коэффициент яркости
        // Используем синусоиду от времени: значение меняется от 0.6 до 1.0
        float t = (float)glfwGetTime();
        float k = 0.8f + 0.2f * sinf(t);

        // Базовый цвет квадрата: (1.0, 0.8, 0.5)
        // Умножаем его на k - становится светлее/темнее
        float r = 1.0f * k;
        float g = 0.8f * k;
        float b = 0.5f * k;

        // Активация шейдера и передача uniform-переменной
        shader.use();
        shader.setVec4("ourColor", r, g, b, 1.0f);

        // Привязываем VAO (в нём сохранены все настройки буферов)
        glBindVertexArray(VAO);
        // Рисуем элементы: 6 индексов (2 треугольника)
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        // Отвязываем VAO
        glBindVertexArray(0);

        // Обмен буферов (отображение нового кадра) и обработка событий
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // 10) Очистка ресурсов
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

    // 11) Завершение работы с контекстом
    glfwTerminate();
    return 0;
}
