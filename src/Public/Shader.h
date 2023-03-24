#pragma once

#include <glm/glm.hpp>
#include <glad/glad.h>

class Shader
{
public:
    // ID program object  
    unsigned int ID;

    // Read shaders from disk and create them
    Shader(const char* vertexPath, const char* fragmentPath, const char* geometryPath = nullptr);
    Shader(const char* ComputePath);

    ~Shader();

    void Use();

    // Setters for uniforms
    void setBool(const char* name, bool value) const;
    void setInt(const char* name, int value) const;
    void setFloat(const char* name, float value) const;
    void setVec2(const char* name, float x, float y) const;
    void setVec2(const char* name, const glm::vec2& vector) const;
    void setVec3(const char* name, float x, float y, float z) const;
    void setVec3(const char* name, const glm::vec3& vector) const;
    void setVec4(const char* name, float x, float y, float z, float w) const;
    void setVec4(const char* name, const glm::vec4& vector) const;
    void setMat4(const char* name, const glm::mat4& value) const;
    void setBlock(const char* name, unsigned int number);

    static void bindUniformData(GLuint UBO, GLuint Offset, GLuint Size, float* Data);

protected:
    Shader() = default;

    void CheckCompileErrors(unsigned int ShaderID, const char* ShaderType);

    static inline unsigned int m_ActiveShader = 0U;
};

