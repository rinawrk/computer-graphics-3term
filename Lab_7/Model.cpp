#include "Model.h"

// Для обработки пути
#include <cstring>

Model::Model(const string& path)
{
    loadModel(path);
}

void Model::Draw() const
{
    // Рендерим все меши модели
    for (unsigned int i = 0; i < meshes.size(); i++)
    {
        meshes[i].Draw();
    }
}

void Model::DrawMesh(unsigned int meshIndex) const
{
    // Защита от выхода за границы массива
    if (meshIndex >= meshes.size())
    {
        cout << "ERROR: mesh index out of range: " << meshIndex << endl;
        return;
    }

    // Рисуем только один нужный меш
    meshes[meshIndex].Draw();
}

void Model::PrintMeshInfo() const
{
    cout << "\n=== Mesh list ===" << endl;

    for (unsigned int i = 0; i < meshes.size(); i++)
    {
        cout << i << " -> " << meshes[i].name << endl;
    }

    cout << "=================\n" << endl;
}

int Model::FindMeshIndexByName(const string& meshName) const
{
    for (unsigned int i = 0; i < meshes.size(); i++)
    {
        if (meshes[i].name == meshName)
        {
            return (int)i;
        }
    }

    return -1;
}

unsigned int Model::GetMeshCount() const
{
    return (unsigned int)meshes.size();
}

void Model::loadModel(const string& path)
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

    // Сохраняем директорию файла
    directory = path.substr(0, path.find_last_of("/\\"));

    // Рекурсивно разбираем дерево узлов
    processNode(scene->mRootNode, scene);

    cout << "Loaded meshes: " << meshes.size() << endl;
}

void Model::processNode(aiNode* node, const aiScene* scene)
{
    // 1) Все меши, которые перечислены в этом узле
    for (unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        unsigned int meshIndex = node->mMeshes[i];
        aiMesh* mesh = scene->mMeshes[meshIndex];

        meshes.push_back(processMesh(mesh, scene, meshIndex));
    }

    // 2) Рекурсивно обрабатываем всех детей узла
    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        processNode(node->mChildren[i], scene);
    }
}

Mesh Model::processMesh(aiMesh* mesh, const aiScene* /*scene*/, unsigned int meshIndex)
{
    vector<Vertex> vertices;
    vector<unsigned int> indices;

    // Индекс материала меша
    unsigned int materialIndex = mesh->mMaterialIndex;
    (void)materialIndex;

    // Берём имя меша из модели
    string meshName = mesh->mName.C_Str();

    // Если имя пустое, задаём запасное имя
    if (meshName.empty())
    {
        meshName = "Mesh_" + to_string(meshIndex);
    }

    // Вершины
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
    // Каждый face после Triangulate = треугольник
    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];

        for (unsigned int j = 0; j < face.mNumIndices; j++)
        {
            indices.push_back(face.mIndices[j]);
        }
    }

    return Mesh(vertices, indices, meshName);
}
