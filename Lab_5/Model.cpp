#include "Model.h"

// Для обработки пути
#include <cstring>

Model::Model(string const& path)
{
    loadModel(path);
}

void Model::Draw()
{
    // Рендерим все меши модели
    for (unsigned int i = 0; i < meshes.size(); i++)
        meshes[i].Draw();
}

void Model::loadModel(string const& path)
{
    Assimp::Importer importer;

    // Загружаем сцену:
    // - Triangulate: приводим все полигоны к треугольникам
    // - GenNormals: генерируем нормали, если их нет
    // - JoinIdenticalVertices: объединяем одинаковые вершины
    const aiScene* scene = importer.ReadFile(
        path,
        aiProcess_Triangulate |
        aiProcess_GenNormals |
        aiProcess_JoinIdenticalVertices
    );

    if (!scene || !scene->mRootNode)
    {
        cout << "ASSIMP ERROR: " << importer.GetErrorString() << endl;
        return;
    }

    // Сохраняем директорию файла (для текстур в будущих лабах)
    directory = path.substr(0, path.find_last_of("/\\"));

    // Рекурсивно разбираем дерево узлов
    processNode(scene->mRootNode, scene);

    cout << "Loaded meshes: " << meshes.size() << endl; // проверка, что модель разбилась на меши
}

void Model::processNode(aiNode* node, const aiScene* scene)
{
    // 1) Все меши, которые перечислены в этом узле
    for (unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(processMesh(mesh, scene));
    }

    // 2) Рекурсивно обрабатываем всех детей узла
    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        processNode(node->mChildren[i], scene);
    }
}

Mesh Model::processMesh(aiMesh* mesh, const aiScene* /*scene*/)
{
    vector<Vertex> vertices;
    vector<unsigned int> indices;

    // Материал меша (пока не используется, но индекс сохраняем как часть структуры Assimp)
    unsigned int materialIndex = mesh->mMaterialIndex;

    // Если позже нужно будет добавлять текстуры/материалы:
    (void)materialIndex; // заглушка, чтобы не было warning "unused variable"

    // Вершины
    // По методичке минимум: Position + Normal
    for (unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        Vertex vertex;

        // Position
        vertex.Position.x = mesh->mVertices[i].x;
        vertex.Position.y = mesh->mVertices[i].y;
        vertex.Position.z = mesh->mVertices[i].z;

        // Normal
        if (mesh->HasNormals())
        {
            vertex.Normal.x = mesh->mNormals[i].x;
            vertex.Normal.y = mesh->mNormals[i].y;
            vertex.Normal.z = mesh->mNormals[i].z;
        }
        else
        {
            vertex.Normal = glm::vec3(0.0f, 0.0f, 0.0f);
        }

        vertices.push_back(vertex);
    }

    // Индексы
    // их берём из faces (каждый face после Triangulate = треугольник)
    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }

    return Mesh(vertices, indices);
}
