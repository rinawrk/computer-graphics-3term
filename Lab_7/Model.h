// На основе Model.h из папки assimp

// Реализация через Model.cpp

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

    // Папка, где лежит файл модели (используется для поиска материалов/текстур при импорте)
    string directory;

    // Конструктор: сразу загружает модель
    Model(string const& path);

    // Отрисовка: просто рисуем все меши
    void Draw();

private:
    // Загружает сцену Assimp и запускает рекурсивный разбор
    void loadModel(string const& path);

    // Рекурсивно обходит дерево узлов и добавляет меши
    void processNode(aiNode* node, const aiScene* scene);

    // Переводит aiMesh в наш Mesh (Vertex + indices + VAO/VBO/EBO)
    Mesh processMesh(aiMesh* mesh, const aiScene* scene);
};

#endif


