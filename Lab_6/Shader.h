#pragma once
#include <string>
#include <GL/glew.h>

// Класс Shader для загрузки, компиляции и использования шейдерной программы
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

    // Передача uniform одной строкой
    void setVec4(const char* name, float x, float y, float z, float w) const;
    void setVec3(const char* name, float x, float y, float z) const;
    void setFloat(const char* name, float v) const;
    void setMat3(const char* name, const float* value) const;
    void setMat4(const char* name, const float* value) const;

private:
    // Прочитать текстовый файл целиком и вернуть строку
    static std::string readTextFile(const char* path);

    // Проверки ошибок компиляции шейдера и линковки программы
    static void checkShader(GLuint shader);
    static void checkProgram(GLuint program);
};

