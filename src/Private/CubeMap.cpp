#include "Public/CubeMap.h"
#include <stb_image.h>

CubeMap::CubeMap(std::vector<const char*> Faces, bool IsStandarised)
    : m_Faces(Faces)
{
    LoadCubeMap(m_Faces, IsStandarised);
}

CubeMap::CubeMap(int Width, int Height)
    : m_Width(Width)
    , m_Height(Height)
{
    LoadCubeMap();
}

CubeMap::CubeMap(const CubeMap& Other)
    : m_Faces(Other.m_Faces)
    , m_Id(Other.m_Id)
{
    const_cast<CubeMap&>(Other).m_Id = 0;
}

CubeMap::CubeMap(CubeMap&& Other) noexcept
    : m_Faces(Other.m_Faces)
    , m_Id(Other.m_Id)
{
    Other.m_Id = 0;
}

CubeMap::~CubeMap()
{
    if (m_Id)
    {
        glDeleteTextures(1, &m_Id);
        m_Id = 0;
        m_Faces.clear();
    }
}

CubeMap& CubeMap::operator=(CubeMap& Other)
{
    if (this != &Other)
    {
        this->~CubeMap();
        std::swap(m_Id, Other.m_Id);
        std::swap(m_Faces, Other.m_Faces);
    }
    return *this;
}

CubeMap& CubeMap::operator=(CubeMap&& Other) noexcept
{
    if (this != &Other)
    {
        this->~CubeMap();
        std::swap(m_Id, Other.m_Id);
        std::swap(m_Faces, Other.m_Faces);
    }
    return *this;
}

void CubeMap::LoadCubeMap(std::vector<const char*> Faces, bool IsStandarised)
{
    glGenTextures(1, &m_Id);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_Id);

    int nrChannels;
    GLenum format, level;
    unsigned char* data;

    for (unsigned int i = 0; i < Faces.size(); ++i)
    {
        data = stbi_load(Faces[i], &m_Width, &m_Height, &nrChannels, 0);
        if (data)
        {
            if (nrChannels == 3)
            {
                format = GL_RGB;
                level = GL_SRGB;
            }
            else if (nrChannels == 1)
            {
                format = GL_RED;
                level = GL_RED;
            }
            else if (nrChannels == 2)
            {
                format = GL_RG;
                level = GL_RG;
            }
            else if (nrChannels == 4)
            {
                format = GL_RGBA;
                level = GL_SRGB_ALPHA;
            }

            glBindTexture(GL_TEXTURE_2D, m_Id);
            if (IsStandarised)
            {
                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, level, m_Width, m_Height, 0, format, GL_UNSIGNED_BYTE, data);
            }
            else
            {
                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, format, m_Width, m_Height, 0, format, GL_UNSIGNED_BYTE, data);
            }

        }
        else
        {
            fprintf(stderr, "Cubemap texture failed to load at path: %s\n", Faces[i]);
        }
        stbi_image_free(data);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void CubeMap::LoadCubeMap()
{
    glGenTextures(1, &m_Id);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_Id);
    for (unsigned int i = 0; i < 6; ++i)
    {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, m_Width, m_Height, 0, GL_RGB, GL_FLOAT, nullptr);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // enable pre-filter mipmap sampling (combatting visible dots artifact)
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

void CubeMap::BindCubeMap(GLuint Number)
{
    if (Number > 31)
    {
        fprintf(stderr, "Failed to bind CubeMap to %d", Number);
        return;
    }
    else
    {
        glActiveTexture(GL_TEXTURE0 + Number);
        glBindTexture(GL_TEXTURE_CUBE_MAP, m_Id);
        glActiveTexture(GL_TEXTURE0);
    }
}

GLuint CubeMap::GetId()
{
    return m_Id;
}

const unsigned int CubeMap::GetWidth() const
{
    return m_Width;
}

const unsigned int CubeMap::GetHeight() const
{
    return m_Height;
}
