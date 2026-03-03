// На основе Mesh.h из папки assimp

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
using namespace std;

class Model
{
public:
    
    vector<Mesh> meshes;
    string directory;
        
    Model(string const& path) 
    {}
       
    void Draw()
    {}

private:
    
    void loadModel(string const& path)
    {}
        
    void processNode(aiNode* node, const aiScene* scene)
    {}

    Mesh processMesh(aiMesh* mesh, const aiScene* scene)
    {
        return Mesh(vertices, indices);
    }        
};

#endif


