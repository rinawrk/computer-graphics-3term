#ifndef MESH_H
#define MESH_H

#include <GL/glew.h>
#include <glm.hpp>                   
#include <gtc/matrix_transform.hpp> 

#include <vector>

using namespace std;

struct Vertex 
{
    // position - координаты вершины в модели
    glm::vec3 Position;
    // normal - нормаль вершины
    glm::vec3 Normal;      
};

class Mesh {
public:
    vector <Vertex> vertices;
    vector <unsigned int> indices;

    // Имя меша из файла модели
    string name;

    unsigned int VAO;

    // Конструктор: сохраняем вершины, индексы и имя меша
    Mesh(vector<Vertex> vertices, vector<unsigned int> indices, const string& name);

    // Отрисовка одного меша
    void Draw() const;

private:
    unsigned int VBO, EBO;

    // Настройка VAO/VBO/EBO
    void setupMesh();
};

#endif
