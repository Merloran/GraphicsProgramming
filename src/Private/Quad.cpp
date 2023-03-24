#include "Public/Quad.h"

Quad& Quad::GetInstance()
{
    static Quad instance;
    return instance;
}

void Quad::Draw(Shader& Shader)
{
    Shader.Use();
    glBindVertexArray(m_VAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}

Quad::~Quad()
{
    glDeleteVertexArrays(1, &m_VAO);
    glDeleteBuffers(1, &m_VBO);
}

Quad::Quad()
{
    float vertices[] =
    {
        //    Position      |       Normal       |   Texture
        -1.0f,  1.0f, 0.0f,   0.0f, 0.0f, -1.0,   0.0f, 1.0f,
        -1.0f, -1.0f, 0.0f,   0.0f, 0.0f, -1.0,   0.0f, 0.0f,
         1.0f,  1.0f, 0.0f,   0.0f, 0.0f, -1.0,   1.0f, 1.0f,
         1.0f, -1.0f, 0.0f,   0.0f, 0.0f, -1.0,   1.0f, 0.0f,
    };

    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);
    glBindVertexArray(m_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
}