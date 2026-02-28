// Лабораторная работа №1: Настройка OpenGL, создание контекста и рендера
// Вариант 10: квадрат, цвет фигуры (1.0, 0.8, 0.5), фон (0.2, 1.0, 1.0)

// Настройка линковки библиотек

/*
В проекте используется статическая линковка GLFW и GLEW.
 - GLFW: подключается через glfw3.lib (статическая версия)
 - GLEW: файл glew.c добавлен в проект (исходный код), поэтому
 определения макросов GLEW_STATIC или GLEW_DLL не требуются.

Для динамической линковки потребовалось бы определить:
#define GLFW_DLL
#define GLEW_DLL
и поместить соответствующие .dll рядом с исполняемым файлом.
*/

#include <GL/glew.h>	 // Заголовочный файл GLEW (управление расширениями OpenGL)
#include <GLFW/glfw3.h>	 // Заголовочный файл GLFW (создание окна, контекста, ввод)

#include <stdio.h>	     // Для вывода сообщений (fprintf, printf)

int main(void)
{
    // Инициализация GLFW
    if (!glfwInit()) {
        fprintf(stderr, "ERROR: could not start GLFW3.\n");
        return 1;
    }

    // Установка версии OpenGL 1.0
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 1);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    // Не используются для OpenGL 1.0:
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_FALSE);
    //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Создание окна и OpenGL контекста
    GLFWwindow* window = glfwCreateWindow(512, 512, "Lab 1 - Variant 10", NULL, NULL);
    if (!window) {
        fprintf(stderr, "ERROR: could not open window with GLFW3.\n");
        glfwTerminate();
        return 1;
    }

    // Делаем созданный контекст текущим
    glfwMakeContextCurrent(window);

    // Инициализация GLEW
    glewExperimental = GL_TRUE; // Разрешаем экспериментальный набор функций
    GLenum ret = glewInit();
    if (GLEW_OK != ret) {
        fprintf(stderr, "Error: %s\n", glewGetErrorString(ret));
        glfwTerminate();
        return 1;
    }

    // Вывод информации о версиях OpenGL и используемом оборудовании
    const GLubyte* version_str = glGetString(GL_VERSION);
    const GLubyte* device_str = glGetString(GL_RENDERER);

    fprintf(stdout, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));
    printf("This version OpenGL running is %s\n", version_str);
    printf("This device OpenGL running is %s\n", device_str);

    // Основной цикл рендеринга
    while (!glfwWindowShouldClose(window)) {

        // Установка цвета фона кадра (вариант 10: (0.2,1.0,1.0))
        glClearColor(0.2f, 1.0f, 1.0f, 1.0f);
        // Очистка кадра
        glClear(GL_COLOR_BUFFER_BIT);

        // Начало отрисовки фигуры (вариант 10: квадрат)
        glBegin(GL_QUADS);

        // Установка цвета фигуры (вариант 10: (1.0,0.8,0.5))
        glColor3f(1.0f, 0.8f, 0.5f);

        // Координаты вершин квадрата
        glVertex2f(-0.5f, 0.5f);     // левая верхняя
        glVertex2f(0.5f, 0.5f);      // правая верхняя
        glVertex2f(0.5f, -0.5f);     // правая нижняя
        glVertex2f(-0.5f, -0.5f);    // левая нижняя

        // Завершение отрисовки
        glEnd();

        // Отображение нового кадра
        glfwSwapBuffers(window);
        // Обработка событий (взаимодействия с окном)
        glfwPollEvents();
    }

    // Завершение работы с контекстом
    glfwTerminate();
    return 0;
}
