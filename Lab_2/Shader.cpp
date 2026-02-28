#include "Shader.h"
#include <fstream>
#include <sstream>
#include <iostream>

std::string Shader::readTextFile(const char* path)
{
    // Открываем файл как обычный текст
    std::ifstream file(path);
    if (!file) {
        std::cerr << "ERROR: cannot open shader file: " << path << "\n";
        return {};
    }

    // Считываем файл целиком в строку
    std::stringstream ss;
    ss << file.rdbuf();
    return ss.str();
}

void Shader::checkShader(GLuint shader)
{
    // Проверяем, скомпилировался ли шейдер
    int ok = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &ok);
    if (!ok) {
        // Если нет — читаем лог ошибок
        char log[1024];
        glGetShaderInfoLog(shader, 1024, nullptr, log);
        std::cerr << "ERROR: shader compile failed:\n" << log << "\n";
    }
}

void Shader::checkProgram(GLuint program)
{
    // Проверяем, залинковалась ли программа (vertex+fragment вместе)
    int ok = 0;
    glGetProgramiv(program, GL_LINK_STATUS, &ok);
    if (!ok) {
        // Если нет — читаем лог ошибок
        char log[1024];
        glGetProgramInfoLog(program, 1024, nullptr, log);
        std::cerr << "ERROR: program link failed:\n" << log << "\n";
    }
}

Shader::Shader(const char* vertexPath, const char* fragmentPath)
{
    // 1) Читаем исходники шейдеров из файлов
    std::string v = readTextFile(vertexPath);
    std::string f = readTextFile(fragmentPath);

    if (v.empty() || f.empty()) {
        std::cerr << "ERROR: shader source is empty.\n";
        ID = 0;
        return;
    }

    const char* vsrc = v.c_str();
    const char* fsrc = f.c_str();

    // 2) Создаём и компилируем вершинный шейдер
    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &vsrc, nullptr);
    glCompileShader(vs);
    checkShader(vs);

    // 3) Создаём и компилируем фрагментный шейдер
    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &fsrc, nullptr);
    glCompileShader(fs);
    checkShader(fs);

    // 4) Создаём программу, "приклеиваем" к ней два шейдера и линкуем
    ID = glCreateProgram();
    glAttachShader(ID, vs);
    glAttachShader(ID, fs);
    glLinkProgram(ID);
    checkProgram(ID);

    // 5) После линковки отдельные объекты шейдеров больше не нужны
    glDeleteShader(vs);
    glDeleteShader(fs);
}

void Shader::use() const
{
    // Сделать эту программу активной (все последующие draw будут с ней)
    glUseProgram(ID);
}

void Shader::setVec4(const char* name, float x, float y, float z, float w) const
{
    // Находим "адрес" uniform переменной по имени
    // (Если вернёт -1, значит имя не найдено или uniform не используется в шейдере)
    GLint loc = glGetUniformLocation(ID, name);

    // Передаём 4 числа (RGBA) в uniform vec4
    glUniform4f(loc, x, y, z, w);
}
