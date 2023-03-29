#include "Public/Mesh.h"

#include "Public/Shader.h"
#include <iostream>
#include <algorithm>

Mesh::Mesh(std::vector<Vertex> vertexes, std::vector<unsigned int> indexes, std::vector<Texture> textures)
    : m_VBO(0)
    , m_VAO(0)
    , m_EBO(0)
    , Vertexes(vertexes)
    , Indexes(indexes)
    , Textures(textures)
{
    if (Mesh::DefaultTextures.empty())
    {
        Texture texture = Texture("res/textures/DefaultTextures/BaseColor.png", true);
        Mesh::DefaultTextures.push_back(texture);

        texture = Texture("res/textures/DefaultTextures/Normal.png", false);
        Mesh::DefaultTextures.push_back(texture);

        texture = Texture("res/textures/DefaultTextures/Emissive.png", false);
        Mesh::DefaultTextures.push_back(texture);

        texture = Texture("res/textures/DefaultTextures/Roughness.png", false);
        Mesh::DefaultTextures.push_back(texture);

        texture = Texture("res/textures/DefaultTextures/Metalness.png", false);
        Mesh::DefaultTextures.push_back(texture);

        texture = Texture("res/textures/DefaultTextures/AO.png", false);
        Mesh::DefaultTextures.push_back(texture);

        for (int i = 0; i < Mesh::DefaultTextures.size(); ++i)
        {
            Mesh::DefaultTextures[i].BindTexture(31 - i);
        }
    }
    SetupMesh();
}

Mesh::Mesh(const Mesh& Other)
    : m_VBO(Other.m_VBO)
    , m_VAO(Other.m_VAO)
    , m_EBO(Other.m_EBO)
    , Vertexes(Other.Vertexes)
    , Indexes(Other.Indexes)
    , Textures(Other.Textures)
{
    const_cast<Mesh&>(Other).m_VBO = 0;
    const_cast<Mesh&>(Other).m_VAO = 0;
    const_cast<Mesh&>(Other).m_EBO = 0;
}

Mesh::Mesh(Mesh&& Other) noexcept
    : m_VBO(Other.m_VBO)
    , m_VAO(Other.m_VAO)
    , m_EBO(Other.m_EBO)
    , Vertexes(Other.Vertexes)
    , Indexes(Other.Indexes)
    , Textures(Other.Textures)
{
    Other.m_VBO = 0;
    Other.m_VAO = 0;
    Other.m_EBO = 0;
}

Mesh::~Mesh()
{
    glDeleteBuffers(1, &m_VBO);
    m_VBO = 0;
    glDeleteBuffers(1, &m_EBO);
    m_EBO = 0;
    glDeleteVertexArrays(1, &m_VAO);
    m_VAO = 0;
    Vertexes.clear();
    Indexes.clear();
    Textures.clear();
}

Mesh& Mesh::operator=(const Mesh& Other)
{
    if (this != &Other)
    {
        this->~Mesh();

        std::swap(m_VBO, const_cast<Mesh&>(Other).m_VBO);
        std::swap(m_VAO, const_cast<Mesh&>(Other).m_VAO);
        std::swap(m_EBO, const_cast<Mesh&>(Other).m_EBO);

        Vertexes = Other.Vertexes;
        Indexes = Other.Indexes;
        Textures = Other.Textures;
    }
    return *this;
}

Mesh& Mesh::operator=(Mesh&& Other) noexcept
{
    if (this != &Other)
    {
        this->~Mesh();

        std::swap(m_VBO, Other.m_VBO);
        std::swap(m_VAO, Other.m_VAO);
        std::swap(m_EBO, Other.m_EBO);

        Vertexes = Other.Vertexes;
        Indexes = Other.Indexes;
        Textures = Other.Textures;
    }
    return *this;
}

void Mesh::SetupMesh()
{
    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);
    glGenBuffers(1, &m_EBO);

    glBindVertexArray(m_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);

    glBufferData(GL_ARRAY_BUFFER, Vertexes.size() * sizeof(Vertex), &Vertexes[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, Indexes.size() * sizeof(unsigned int), &Indexes[0], GL_STATIC_DRAW);
    
    // Position attribute
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    // Normal attribute
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
    // Texture position attribute
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Mesh::ResetTextures(Shader& Shader)
{
    for (int i = 0; i < Mesh::DefaultTextures.size(); ++i)
    {
        Mesh::DefaultTextures[i].BindTexture(31 - i);
    }
    Shader.setInt("material.albedo[0]"          , 31);
    Shader.setInt("material.normal[0]"          , 30);
    Shader.setInt("material.emission[0]"        , 29);
    Shader.setInt("material.metalness[0]"       , 28);
    Shader.setInt("material.roughness[0]"       , 27);
    Shader.setInt("material.ambientocclusion[0]", 26);
}

void Mesh::Draw(Shader& Shader, unsigned int Amount)
{
    ResetTextures(Shader);
    unsigned int textureNrs[(int)TextureType::TYPESCOUNT];
    std::fill(textureNrs, textureNrs + (int)TextureType::TYPESCOUNT, 0U);
    std::string number;
    TextureType type;
    std::string name;
    ResetTextures(Shader);
    for (unsigned int i = 0; i < Textures.size(); ++i)
    {
        Textures[i].BindTexture(i);
        type = Textures[i].GetType();

        number = std::to_string(textureNrs[(int)type]++);

        switch (type)
        {
            case TextureType::ALBEDO:
            {
                name = "albedo";
                break;
            }
            case TextureType::NORMAL:
            {
                name = "normal";
                break;
            }
            case TextureType::EMISSION:
            {
                name = "emission";
                break;
            }
            case TextureType::METALNESS:
            {
                name = "metalness";
                break;
            }
            case TextureType::ROUGHNESS:
            {
                name = "roughness";
                break;
            }
            case TextureType::AMBIENTOCCLUSION:
            {
                name = "ambientocclusion";
                break;
            }
            case TextureType::NONE:
            case TextureType::TYPESCOUNT:
            default:
                break;
        }

        Shader.setInt(("material." + name + '[' + number + ']').c_str(), i);
    }

    glBindVertexArray(m_VAO);
    if (Amount == 1U)
    {
        glDrawElements(GL_TRIANGLES, Indexes.size(), GL_UNSIGNED_INT, 0);
    }
    else
    {
        glDrawElementsInstanced(GL_TRIANGLES, Indexes.size(), GL_UNSIGNED_INT, 0, Amount);
    }

    glBindVertexArray(0);
    glActiveTexture(GL_TEXTURE0);
}

unsigned int Mesh::GetVAO()
{
    return m_VAO;
}

unsigned int Mesh::GetVBO()
{
    return m_VBO;
}

unsigned int Mesh::GetEBO()
{
    return m_EBO;
}
