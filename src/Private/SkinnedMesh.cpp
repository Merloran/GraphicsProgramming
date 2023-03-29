#include "Public/SkinnedMesh.h"
#include "Public/Shader.h"


SkinnedMesh::SkinnedMesh(std::vector<SkinnedVertex> vertexes, std::vector<uint32_t> indexes, std::vector<Texture> textures)
    : m_VBO(0)
    , m_VAO(0)
    , m_EBO(0)
    , Vertexes(vertexes)
    , Indexes(indexes)
    , Textures(textures)
{
    if (DefaultTextures.empty())
    {
        Texture texture = Texture("res/textures/DefaultTextures/BaseColor.png", true);
        DefaultTextures.push_back(texture);

        texture = Texture("res/textures/DefaultTextures/Normal.png", false);
        DefaultTextures.push_back(texture);

        texture = Texture("res/textures/DefaultTextures/Emissive.png", false);
        DefaultTextures.push_back(texture);

        texture = Texture("res/textures/DefaultTextures/Roughness.png", false);
        DefaultTextures.push_back(texture);

        texture = Texture("res/textures/DefaultTextures/Metalness.png", false);
        DefaultTextures.push_back(texture);

        texture = Texture("res/textures/DefaultTextures/AO.png", false);
        DefaultTextures.push_back(texture);

        for (int i = 0; i < DefaultTextures.size(); ++i)
        {
            DefaultTextures[i].BindTexture(31 - i);
        }
    }
    SetupMesh();
}

SkinnedMesh::SkinnedMesh(const SkinnedMesh& Other)
    : m_VBO(Other.m_VBO)
    , m_VAO(Other.m_VAO)
    , m_EBO(Other.m_EBO)
    , Vertexes(Other.Vertexes)
    , Indexes(Other.Indexes)
    , Textures(Other.Textures)
{
    const_cast<SkinnedMesh&>(Other).m_VBO = 0;
    const_cast<SkinnedMesh&>(Other).m_VAO = 0;
    const_cast<SkinnedMesh&>(Other).m_EBO = 0;
}

SkinnedMesh::SkinnedMesh(SkinnedMesh&& Other) noexcept
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

SkinnedMesh::~SkinnedMesh()
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

SkinnedMesh& SkinnedMesh::operator=(const SkinnedMesh& Other)
{
    if (this != &Other)
    {
        this->~SkinnedMesh();

        std::swap(m_VBO, const_cast<SkinnedMesh&>(Other).m_VBO);
        std::swap(m_VAO, const_cast<SkinnedMesh&>(Other).m_VAO);
        std::swap(m_EBO, const_cast<SkinnedMesh&>(Other).m_EBO);

        Vertexes = Other.Vertexes;
        Indexes = Other.Indexes;
        Textures = Other.Textures;
    }
    return *this;
}

SkinnedMesh& SkinnedMesh::operator=(SkinnedMesh&& Other) noexcept
{
    if (this != &Other)
    {
        this->~SkinnedMesh();

        std::swap(m_VBO, Other.m_VBO);
        std::swap(m_VAO, Other.m_VAO);
        std::swap(m_EBO, Other.m_EBO);

        Vertexes = Other.Vertexes;
        Indexes = Other.Indexes;
        Textures = Other.Textures;
    }
    return *this;
}

void SkinnedMesh::Draw(Shader& Shader, uint32_t Amount)
{
    ResetTextures(Shader);
    uint32_t textureNrs[(int32_t)TextureType::TYPESCOUNT];
    std::fill(textureNrs, textureNrs + (int32_t)TextureType::TYPESCOUNT, 0U);
    std::string number;
    TextureType type;
    std::string name;
    ResetTextures(Shader);
    for (uint32_t i = 0; i < Textures.size(); ++i)
    {
        Textures[i].BindTexture(i);
        type = Textures[i].GetType();

        number = std::to_string(textureNrs[(int32_t)type]++);

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

void SkinnedMesh::ResetTextures(Shader& Shader)
{
    for (int i = 0; i < DefaultTextures.size(); ++i)
    {
        DefaultTextures[i].BindTexture(31 - i);
    }
    Shader.setInt("material.albedo[0]", 31);
    Shader.setInt("material.normal[0]", 30);
    Shader.setInt("material.emission[0]", 29);
    Shader.setInt("material.metalness[0]", 28);
    Shader.setInt("material.roughness[0]", 27);
    Shader.setInt("material.ambientocclusion[0]", 26);
}

void SkinnedMesh::SetupMesh()
{
    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);
    glGenBuffers(1, &m_EBO);

    glBindVertexArray(m_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);

    glBufferData(GL_ARRAY_BUFFER, Vertexes.size() * sizeof(SkinnedVertex), &Vertexes[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, Indexes.size() * sizeof(unsigned int), &Indexes[0], GL_STATIC_DRAW);

    // Position attribute
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(SkinnedVertex), (void*)0);
    // Normal attribute
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(SkinnedVertex), (void*)offsetof(SkinnedVertex, Normal));
    // Texture position attribute
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(SkinnedVertex), (void*)offsetof(SkinnedVertex, TexCoords));
    // Bone IDs
    glEnableVertexAttribArray(4);
    glVertexAttribIPointer(4, 4, GL_INT,            sizeof(SkinnedVertex), (void*)offsetof(SkinnedVertex, BoneIDs));
    // Vertex weights
    glEnableVertexAttribArray(5);
    glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(SkinnedVertex), (void*)offsetof(SkinnedVertex, Weights));

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

uint32_t SkinnedMesh::GetVAO()
{
    return m_VAO;
}

uint32_t SkinnedMesh::GetVBO()
{
    return m_VBO;
}

uint32_t SkinnedMesh::GetEBO()
{
    return m_EBO;
}
