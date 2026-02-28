#pragma once
#include <string>
#include <GL/glew.h>

// Класс Shader — маленькая "библиотека" для задания 2:
// - читает .vert/.frag из файлов
// - компилирует и линкует шейдерную программу
// - позволяет задавать uniform в одну строку

class Shader
{
public:
    // ID шейдерной программы (то, что возвращает glCreateProgram)
    GLuint ID = 0;

    // Конструктор: собирает программу из двух файлов
    // vertexPath  — путь к .vert
    // fragmentPath — путь к .frag
    Shader(const char* vertexPath, const char* fragmentPath);

    // Включить (сделать активной) эту программу шейдеров
    void use() const;

    // Передать uniform vec4 (RGBA цвет) одной строкой
    void setVec4(const char* name, float x, float y, float z, float w) const;

private:
    // Прочитать текстовый файл целиком и вернуть строку
    static std::string readTextFile(const char* path);

    // Проверки ошибок компиляции шейдера и линковки программы
    static void checkShader(GLuint shader);
    static void checkProgram(GLuint program);
};

