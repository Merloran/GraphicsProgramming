#pragma once
#include <vector>
#include "glm/glm.hpp"
#include "Public/Texture.h"

const int MAX_BONE_INFLUENCE = 4;

struct SkinnedVertex
{
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
    int32_t BoneIDs[MAX_BONE_INFLUENCE];
    float Weights[MAX_BONE_INFLUENCE];
};


class Vertex;
class Shader;
class Texture;

class SkinnedMesh
{
public:
    std::vector<SkinnedVertex> Vertexes;
    std::vector<uint32_t> Indexes;
    std::vector<Texture> Textures;

    static inline std::vector<Texture> DefaultTextures = {};

    SkinnedMesh(std::vector<SkinnedVertex> vertexes, std::vector<uint32_t> indexes, std::vector<Texture> textures);
    SkinnedMesh(const SkinnedMesh& Other);
    SkinnedMesh(SkinnedMesh&& Other) noexcept;

    ~SkinnedMesh();

    SkinnedMesh& operator=(const SkinnedMesh& Other);
    SkinnedMesh& operator=(SkinnedMesh&& Other) noexcept;

    void Draw(Shader& Shader, uint32_t Amount = 1U);

    static void ResetTextures(Shader& Shader);

    uint32_t GetVAO();
    uint32_t GetVBO();
    uint32_t GetEBO();

protected:
    uint32_t m_VBO, m_VAO, m_EBO;
    void SetupMesh();
};