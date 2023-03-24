#pragma once

#include <string>
#include <vector>
#include <glad/glad.h>

#include "Mesh.h"
#include "Texture.h"
#include "Object.h"

class Shader;

struct aiNode;
struct aiScene;
struct aiMesh;
struct aiMaterial;

enum aiTextureType;

class Model : public Object
{
public:
    Model(const char* Path);
    virtual void Draw(Shader& Shader) override;

    Mesh& GetMesh(unsigned int Index);

    unsigned int GetMeshCount() const;

protected:
    std::vector<Mesh> m_Meshes;

private:
    std::vector<Texture> m_TexturesLoaded;
    std::string m_Directory;

    void LoadModel(std::string path);
    void ProcessNode(aiNode* node, const aiScene* scene);
    Mesh ProcessMesh(aiMesh* mesh, const aiScene* scene);
    std::vector<Texture> LoadMaterialTextures(aiMaterial* material, aiTextureType type, TextureType typeName);
};