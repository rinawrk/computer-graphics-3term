#ifndef MODEL_H
#define MODEL_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Mesh.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

using namespace std;

class Model
{
public:
    // Набор мешей, из которых состоит модель
    vector<Mesh> meshes;

    // Папка, где лежит файл модели
    string directory;

    // Конструктор: сразу загружает модель
    Model(const string& path);

    // Отрисовка всей модели целиком
    void Draw() const;

    // Отрисовка одного меша по индексу
    void DrawMesh(unsigned int meshIndex) const;

    // Вывести в консоль индексы и имена мешей
    void PrintMeshInfo() const;

    // Найти индекс меша по имени
    int FindMeshIndexByName(const string& meshName) const;

    // Количество мешей в модели
    unsigned int GetMeshCount() const;

private:
    // Загружает сцену Assimp и запускает рекурсивный разбор
    void loadModel(const string& path);

    // Рекурсивно обходит дерево узлов и добавляет меши
    void processNode(aiNode* node, const aiScene* scene);

    // Переводит aiMesh в наш Mesh
    Mesh processMesh(aiMesh* mesh, const aiScene* scene, unsigned int meshIndex);
};

#endif
