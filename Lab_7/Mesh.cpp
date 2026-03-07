#include "Mesh.h"

// Нужен для offsetof(Vertex, Normal)
#include <cstddef>

// Конструктор меша: сохраняем данные и настраиваем OpenGL буферы
Mesh::Mesh(vector<Vertex> vertices, vector<unsigned int> indices, const string& name)
{
    this->vertices = vertices;
    this->indices = indices;
    this->name = name;

    // Создаём VAO/VBO/EBO и настраиваем атрибуты вершин
    setupMesh();
}

// Настройка меша на стороне OpenGL:
// - VAO хранит "описание" атрибутов
// - VBO хранит массив вершин
// - EBO хранит индексы (какие вершины образуют треугольники)
void Mesh::setupMesh()
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    // VBO: вершины
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER,
        vertices.size() * sizeof(Vertex),
        vertices.data(),
        GL_STATIC_DRAW);

    // EBO: индексы
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
        indices.size() * sizeof(unsigned int),
        indices.data(),
        GL_STATIC_DRAW);

    // Атрибут 0: Position (vec3)
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0,
        3, GL_FLOAT, GL_FALSE,
        sizeof(Vertex),
        (void*)0);

    // Атрибут 1: Normal (vec3)
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1,
        3, GL_FLOAT, GL_FALSE,
        sizeof(Vertex),
        (void*)offsetof(Vertex, Normal));

    glBindVertexArray(0);
}

// Отрисовка меша:
// шейдер активируется снаружи (в main), здесь Mesh выполняет draw-вызов
void Mesh::Draw() const
{
    glBindVertexArray(VAO);

    // Рисуем треугольники по индексам
    glDrawElements(GL_TRIANGLES,
        (GLsizei)indices.size(),
        GL_UNSIGNED_INT,
        0);

    glBindVertexArray(0);
}
