#include "Public/InstancedModel.h"

InstancedModel::InstancedModel(const char* Path, std::vector<glm::mat4> Transforms)
	: Model(Path)
    , m_ElementsCount(Transforms.size())
{
    glGenBuffers(1, &m_InstanceVBO);
    glBindBuffer(GL_ARRAY_BUFFER, m_InstanceVBO);

    glBufferData(GL_ARRAY_BUFFER, m_ElementsCount * sizeof(glm::mat4), &Transforms[0], GL_STATIC_DRAW);
    for (Mesh& mesh : m_Meshes)
    {
        glBindVertexArray(mesh.GetVAO());
        // Atrybuty wierzcho³ków
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)0);
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4)));
        glEnableVertexAttribArray(5);
        glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4)));
        glEnableVertexAttribArray(6);
        glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4)));

        glVertexAttribDivisor(3, 1);
        glVertexAttribDivisor(4, 1);
        glVertexAttribDivisor(5, 1);
        glVertexAttribDivisor(6, 1);

        glBindVertexArray(0);
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

InstancedModel::~InstancedModel() = default;

void InstancedModel::Draw(Shader& Shader)
{
    Shader.Use();

    for (Mesh& mesh : m_Meshes)
    {
        mesh.Draw(Shader, m_ElementsCount);
    }
}
