#pragma once
#include <string>
#include <iostream>
#include <glad/glad.h>


enum class TextureType : unsigned char
{
    NONE,
    ALBEDO,
    NORMAL,
    EMISSION,
    METALNESS,
    ROUGHNESS,
    AMBIENTOCCLUSION,
    TYPESCOUNT, // Number of types in enum
};

class Texture
{
public:

    Texture();
    // Loading Standard texture with saved type
    Texture(TextureType Type, std::string Path, bool IsStandarised = false);
    // Loading Standard texture
    Texture(std::string Path, bool IsStandarised);
    // Loading HDR texture
    Texture(std::string Path);
    // Create custom texture
    Texture(int Width, int Height, int NrChannels);

    //Texture(const Texture& Other);
    //Texture(Texture&& Other) noexcept;

    //~Texture();

    //Texture& operator=(Texture& Other);
    //Texture& operator=(Texture&& Other) noexcept;

    // Bind texture to specified texture
    void BindTexture(GLuint Number);

    GLuint GetId();
    TextureType GetType() const;
    std::string GetPath() const;
    int GetWidth() const;
    int GetHeight() const;

private:
    void LoadTexture(bool IsStandarised = false);
    void LoadTextureHDR();
    void GenerateTexture();

    GLuint m_Id;
    TextureType m_Type;
    std::string m_Path;
    int m_Height, m_Width, m_NrChannels;
};

