#pragma once

#include <vector>
#include <string>
#include <glm/glm.hpp>
#include <glad/glad.h>
#include "Texture.h"

class Shader;

struct Vertex
{
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
};

class Mesh
{
public:
    std::vector<Vertex> Vertexes;
    std::vector<unsigned int> Indexes;
    std::vector<Texture> Textures;

    static inline std::vector<Texture> DefaultTextures = {};

    Mesh(std::vector<Vertex> vertexes, std::vector<unsigned int> indexes, std::vector<Texture> textures);
    Mesh(const Mesh& Other);
    Mesh(Mesh&& Other) noexcept;

    ~Mesh();

    Mesh& operator=(const Mesh& Other);
    Mesh& operator=(Mesh&& Other) noexcept;

    void Draw(Shader& Shader, unsigned int Amount = 1U);

    static void ResetTextures(Shader& Shader);

    unsigned int GetVAO();
    unsigned int GetVBO();
    unsigned int GetEBO();

protected:
    unsigned int m_VBO, m_VAO, m_EBO;
    virtual void SetupMesh();
};

