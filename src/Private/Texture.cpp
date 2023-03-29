#include "../Public/Texture.h"
#include <stb_image.h>


Texture::Texture() = default;

Texture::Texture(TextureType Type, std::string Path, bool IsStandarised)
	: m_Type(Type)
	, m_Path(Path)
{
    LoadTexture(IsStandarised);
}

Texture::Texture(std::string Path, bool IsStandarised)
    : m_Path(Path)
    , m_Type(TextureType::NONE)
{
    LoadTexture(IsStandarised);
}

Texture::Texture(std::string Path)
    : m_Path(Path)
    , m_Type(TextureType::NONE)
{
    LoadTextureHDR();
}

Texture::Texture(int Width, int Height, int NrChannels)
    : m_Width(Width)
    , m_Height(Height)
    , m_NrChannels(NrChannels)
{
}

//Texture::Texture(const Texture& Other)
//    : m_Id(Other.m_Id)
//    , m_Type(Other.m_Type)
//    , m_Path(Other.m_Path)
//{
//    const_cast<Texture&>(Other).m_Id = 0;
//}
//
//Texture::Texture(Texture&& Other) noexcept
//	: m_Id(Other.m_Id)
//	, m_Type(Other.m_Type)
//	, m_Path(Other.m_Path)
//{
//	Other.m_Id = 0;
//}
//
//Texture::~Texture()
//{
//    if (m_Id)
//    {
//        glDeleteTextures(1, &m_Id);
//        m_Id = 0;
//        m_Path.clear();
//        m_Type = TextureType::NONE;
//    }
//}
//
//Texture& Texture::operator=(Texture& Other)
//{
//    if (this != &Other)
//    {
//        this->~Texture();
//        std::swap(m_Id, Other.m_Id);
//        std::swap(m_Type, Other.m_Type);
//        std::swap(m_Path, Other.m_Path);
//    }
//    return *this;
//}
//
//Texture& Texture::operator=(Texture&& Other) noexcept
//{
//    if (this != &Other)
//    {
//        this->~Texture();
//        std::swap(m_Id, Other.m_Id);
//        std::swap(m_Type, Other.m_Type);
//        std::swap(m_Path, Other.m_Path);
//    }
//    return *this;
//}


void Texture::BindTexture(GLuint Number)
{
    if (Number > 31)
    {
        fprintf(stderr, "Failed to bind texture to %d", Number);
        return;
    }
    else
    {
        glActiveTexture(GL_TEXTURE0 + Number);
        glBindTexture(GL_TEXTURE_2D, m_Id);
        glActiveTexture(GL_TEXTURE0);
    }
}

GLuint Texture::GetId()
{
    return m_Id;
}

TextureType Texture::GetType() const
{
    return m_Type;
}

std::string Texture::GetPath() const
{
    return m_Path;
}

int Texture::GetWidth() const
{
    return m_Width;
}

int Texture::GetHeight() const
{
    return m_Height;
}


void Texture::LoadTexture(bool IsStandarised)
{
    unsigned char* data;

    glGenTextures(1, &m_Id);

    data = stbi_load(m_Path.c_str(), &m_Width, &m_Height, &m_NrChannels, 0);

    if (data)
    {
        GLenum format, level;
        switch (m_NrChannels)
        {
            case 1:
            {
                format = GL_RED;
                level = GL_RED;
                break;
            }
            case 2:
            {
                format = GL_RG;
                level = GL_RG;
                break;
            }
            case 4:
            {
                format = GL_RGBA;
                level = GL_SRGB_ALPHA;
                break;
            }
            case 3:
            default:
            {
                format = GL_RGB;
                level = GL_SRGB;
                break;
            }
        }

        glBindTexture(GL_TEXTURE_2D, m_Id);
        if (IsStandarised)
        {
            glTexImage2D(GL_TEXTURE_2D, 0, level, m_Width, m_Height, 0, format, GL_UNSIGNED_BYTE, data);
        }
        else
        {
            glTexImage2D(GL_TEXTURE_2D, 0, format, m_Width, m_Height, 0, format, GL_UNSIGNED_BYTE, data);
        }
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
    else
    {
        fprintf(stderr, "Failed to load texture %s", m_Path.c_str());
        m_Type = TextureType::NONE;
    }
    glBindTexture(GL_TEXTURE_2D, 0);
    stbi_image_free(data);
}

void Texture::LoadTextureHDR()
{
    stbi_set_flip_vertically_on_load(true);
    float* data = stbi_loadf(m_Path.c_str(), &m_Width, &m_Height, &m_NrChannels, 0);
    if (data)
    {
        GLenum format, level;
        switch (m_NrChannels)
        {
            case 1:
            {
                format = GL_R16F;
                level = GL_RED;
                break;
            }
            case 2:
            {
                format = GL_RG16F;
                level = GL_RG;
                break;
            }
            case 4:
            {
                format = GL_RGBA16F;
                level = GL_RGBA;
                break;
            }
            case 3:
            default:
            {
                format = GL_RGB16F;
                level = GL_RGB;
                break;
            }
        }

        glGenTextures(1, &m_Id);
        glBindTexture(GL_TEXTURE_2D, m_Id);
        glTexImage2D(GL_TEXTURE_2D, 0, format, m_Width, m_Height, 0, level, GL_FLOAT, data); // note how we specify the texture's data value to be float

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
    else
    {
        fprintf(stderr, "Failed to load texture %s", m_Path.c_str());
    }
    glBindTexture(GL_TEXTURE_2D, 0);
    stbi_image_free(data);
}

void Texture::GenerateTexture()
{
    GLenum format, level;
    switch (m_NrChannels)
    {
        case 1:
        {
            format = GL_R16F;
            level = GL_RED;
            break;
        }
        case 2:
        {
            format = GL_RG16F;
            level = GL_RG;
            break;
        }
        case 4:
        {
            format = GL_RGBA16F;
            level = GL_RGBA;
            break;
        }
        case 3:
        default:
        {
            format = GL_RGB16F;
            level = GL_RGB;
            break;
        }
    }

    glGenTextures(1, &m_Id);

    // pre-allocate enough memory for the LUT texture.
    glBindTexture(GL_TEXTURE_2D, m_Id);
    glTexImage2D(GL_TEXTURE_2D, 0, format, 512, 512, 0, level, GL_FLOAT, 0);
    // be sure to set wrapping mode to GL_CLAMP_TO_EDGE
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindTexture(GL_TEXTURE_2D, 0);
}