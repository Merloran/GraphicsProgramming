#pragma once
#include <vector>
#include <string>
#include <glad/glad.h>

class CubeMap
{

public:
    CubeMap(std::vector<const char*> Faces, bool IsStandarised = false);
    CubeMap(int Width, int Height);

    CubeMap(const CubeMap& Other);
    CubeMap(CubeMap&& Other) noexcept;

    ~CubeMap();

    CubeMap& operator=(CubeMap& Other);
    CubeMap& operator=(CubeMap&& Other) noexcept;

    /** Load cube map from set of 6 textures */
	void LoadCubeMap(std::vector<const char*> Faces, bool IsStandarised = false);
    /** Load cube map from hdr file */
	void LoadCubeMap();

    void BindCubeMap(GLuint Number);

    GLuint GetId();
    const unsigned int GetWidth() const;
    const unsigned int GetHeight() const;

private:
    GLuint m_Id; 
	std::vector<const char*> m_Faces;
    int m_Width, m_Height;
};