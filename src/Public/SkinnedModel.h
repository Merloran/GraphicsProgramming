#pragma once
#include "Object.h"
#include "SkinnedMesh.h"
#include "Texture.h"
#include "BoneInfo.h"
#include <unordered_map>

class Shader;

struct aiNode;
struct aiScene;
struct aiMesh;
struct aiMaterial;

enum aiTextureType;

class SkinnedModel : public Object
{
public:
    SkinnedModel(const char* Path);
    virtual void Draw(Shader& Shader) override;

    SkinnedMesh& GetMesh(uint32_t Index);

    uint32_t GetMeshCount() const;
    auto& GetBoneInfoMap() { return m_BoneInfoMap; }
    int32_t& GetBoneCount() { return m_BoneCounter; }
protected:
    std::vector<SkinnedMesh> m_Meshes;

private:
    std::unordered_map<std::string, BoneInfo> m_BoneInfoMap;
    int32_t m_BoneCounter = 0;
    std::vector<Texture> m_TexturesLoaded;
    std::string m_Directory;

    void SetVertexBoneDataToDefault(SkinnedVertex& vertex);
    void SetVertexBoneData(SkinnedVertex& vertex, int32_t boneID, float weight);
    void LoadModel(std::string path);
    void ProcessNode(aiNode* node, const aiScene* scene);
    SkinnedMesh ProcessMesh(aiMesh* mesh, const aiScene* scene);
    void ExtractBoneWeightForVertices(std::vector<SkinnedVertex>& vertices, aiMesh* mesh, const aiScene* scene);
    std::vector<Texture> LoadMaterialTextures(aiMaterial* material, aiTextureType type, TextureType typeName);
};

