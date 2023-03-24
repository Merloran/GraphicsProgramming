#include "../Public/Shader.h"

#include <glm/gtc/type_ptr.hpp>
#include <fstream>
#include <sstream>

Shader::Shader(const char* vertexPath, const char* fragmentPath, const char* geometryPath)
{
    std::string vShaderCode;
    std::string fShaderCode;
    std::string gShaderCode;
    std::ifstream vShaderFile;
    std::ifstream fShaderFile;
    std::ifstream gShaderFile;

    // zapewnij by obiekt ifstream móg³ rzucaæ wyj¹tkami  
    vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    try
    {
        // Open files
        vShaderFile.open(vertexPath);
        fShaderFile.open(fragmentPath);
        std::stringstream vShaderStream, fShaderStream, gShaderStream;
        // zapisz zawartoœæ bufora pliku do strumieni  
        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();
        // zamknij uchtywy do plików  
        vShaderFile.close();
        fShaderFile.close();
        // zamieñ strumieñ w ³añcuch znaków 
        vShaderCode = vShaderStream.str();
        fShaderCode = fShaderStream.str();

        if (geometryPath)
        {
            gShaderFile.open(geometryPath);
            gShaderStream << gShaderFile.rdbuf();
            gShaderFile.close();
            gShaderCode = gShaderStream.str();
        }

    }
    catch (std::ifstream::failure& e)
    {
        fprintf(stderr, "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ: %s\n", e.what());
    }
    const char *vertexShaderCode, *fragmentShaderCode, *geometryShaderCode;

    vertexShaderCode = vShaderCode.c_str();
    fragmentShaderCode = fShaderCode.c_str();
    if (geometryPath)
    {
        geometryShaderCode = gShaderCode.c_str();
    }

    unsigned int vertex, fragment, geometry;

    // Vertex Shader  
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vertexShaderCode, NULL);
    glCompileShader(vertex);
    CheckCompileErrors(vertex, "VERTEX");

    // Fragment Shader  
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fragmentShaderCode, NULL);
    glCompileShader(fragment);
    CheckCompileErrors(fragment, "FRAGMENT");

    if (geometryPath)
    {
        // Geometry Shader  
        geometry = glCreateShader(GL_GEOMETRY_SHADER);
        glShaderSource(geometry, 1, &geometryShaderCode, NULL);
        glCompileShader(geometry);
        CheckCompileErrors(geometry, "GEOMETRY");
    }

    // Program Object  
    ID = glCreateProgram();
    glAttachShader(ID, vertex);
    glAttachShader(ID, fragment);
    if (geometryPath)
    {
        glAttachShader(ID, geometry);
    }
    glLinkProgram(ID);
    CheckCompileErrors(ID, "PROGRAM");

    // Deleting shaders
    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

Shader::Shader(const char* ComputePath)
{
    // 1. retrieve the vertex/fragment source code from filePath
    std::string computeCode;
    std::ifstream cShaderFile;
    // ensure ifstream objects can throw exceptions:
    cShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try
    {
        // open files
        cShaderFile.open(ComputePath);

        std::stringstream cShaderStream;
        // read file's buffer contents into streams
        cShaderStream << cShaderFile.rdbuf();
        // close file handlers
        cShaderFile.close();
        // convert stream into string
        computeCode = cShaderStream.str();
    }
    catch (std::ifstream::failure& e)
    {
        fprintf(stderr, "ERROR::COMPUTE::SHADER::FILE_NOT_SUCCESFULLY_READ: %s\n", e.what());
    }
    const char* cShaderCode = computeCode.c_str();
    // 2. compile shaders
    unsigned int compute;
    // compute shader
    compute = glCreateShader(GL_COMPUTE_SHADER);
    glShaderSource(compute, 1, &cShaderCode, NULL);
    glCompileShader(compute);
    CheckCompileErrors(compute, "COMPUTE");

    // shader Program
    ID = glCreateProgram();
    glAttachShader(ID, compute);
    glLinkProgram(ID);
    CheckCompileErrors(ID, "PROGRAM");
    // delete the shaders as they're linked into our program now and no longer necessery
    glDeleteShader(compute);
}

Shader::~Shader()
{
    if (ID)
    {
        glDeleteProgram(ID);
    }
}

void Shader::Use()
{
    if (m_ActiveShader != ID)
    {
        glUseProgram(ID);
        m_ActiveShader = ID;
    }
}

void Shader::setBool(const char* name, bool value) const
{
    glUniform1i(glGetUniformLocation(ID, name), (int)value);
}

void Shader::setInt(const char* name, int value) const
{
    glUniform1i(glGetUniformLocation(ID, name), value);
}

void Shader::setFloat(const char* name, float value) const
{
    glUniform1f(glGetUniformLocation(ID, name), value);
}

void Shader::setVec2(const char* name, float x, float y) const
{
    glUniform2f(glGetUniformLocation(ID, name), x, y);
}

void Shader::setVec2(const char* name, const glm::vec2& vector) const
{
    glUniform2f(glGetUniformLocation(ID, name), vector.x, vector.y);
}

void Shader::setVec3(const char* name, float x, float y, float z) const
{
    glUniform3f(glGetUniformLocation(ID, name), x, y, z);
}

void Shader::setVec3(const char* name, const glm::vec3& vector) const
{
    glUniform3f(glGetUniformLocation(ID, name), vector.x, vector.y, vector.z);
}

void Shader::setVec4(const char* name, float x, float y, float z, float w) const
{
    glUniform4f(glGetUniformLocation(ID, name), x, y, z, w);
}

void Shader::setVec4(const char* name, const glm::vec4& vector) const
{
    glUniform4f(glGetUniformLocation(ID, name), vector.x, vector.y, vector.z, vector.w);
}

void Shader::setMat4(const char* name, const glm::mat4& value) const
{
    glUniformMatrix4fv(glGetUniformLocation(ID, name), 1, GL_FALSE, glm::value_ptr(value));
}

void Shader::setBlock(const char* name, unsigned int number)
{
    glUniformBlockBinding(ID, glGetUniformBlockIndex(ID, name), number);
}

void Shader::bindUniformData(GLuint UBO, GLuint Offset, GLuint Size, float* Data)
{
    glBindBuffer(GL_UNIFORM_BUFFER, UBO);
    glBufferSubData(GL_UNIFORM_BUFFER, Offset, Size, Data);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void Shader::CheckCompileErrors(unsigned int ShaderID, const char* ShaderType)
{
    GLint success;
    GLchar infoLog[1024];
    if (ShaderType != "PROGRAM")
    {
        glGetShaderiv(ShaderID, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(ShaderID, 1024, NULL, infoLog);
            fprintf(stderr, "ERROR::SHADER_COMPILATION_ERROR::%s\n%s \n -- --------------------------------------------------- -- \n", ShaderType, infoLog);
        }
    }
    else
    {
        glGetProgramiv(ShaderID, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(ShaderID, 1024, NULL, infoLog);
            fprintf(stderr, "ERROR::PROGRAM_LINKING_ERROR::%s\n%s \n -- --------------------------------------------------- -- \n", ShaderType, infoLog);
        }
    }
}
