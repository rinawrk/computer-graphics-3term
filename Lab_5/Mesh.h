// На основе Mesh.h из папки assimp

// Делаем через Mesh.cpp, так как это правильнее по C++: заголовок содержит объявления, cpp - реализацию

#ifndef MESH_H
#define MESH_H

#include <GL/glew.h> 
#include <glm.hpp> 
#include <gtc/matrix_transform.hpp>

#include <vector>

using namespace std;

struct Vertex {
    // position
    glm::vec3 Position;
    // normal
    glm::vec3 Normal;      
};

class Mesh {
public:
    vector <Vertex> vertices;
    vector <unsigned int> indices;
    unsigned int VAO;

    // Конструктор (реализация в Mesh.cpp)
    Mesh(vector<Vertex> vertices, vector<unsigned int> indices);

    // Отрисовка меша (реализация в Mesh.cpp)
    void Draw();

private:
    unsigned int VBO, EBO;

    // Настройка VAO/VBO/EBO (реализация в Mesh.cpp)
    void setupMesh();
};

#endif


