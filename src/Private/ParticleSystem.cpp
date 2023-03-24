#include "Public/ParticleSystem.h"
#include "Public/Quad.h"

ParticleSystem::ParticleSystem(Texture& texture, GLuint amount, float maxLifeTime)
    : m_Texture(&texture)
    , m_Amount(amount)
    , m_MaxLifeTime(maxLifeTime)
{
    this->init();
}

ParticleSystem::~ParticleSystem()
{
    glDeleteBuffers(3, m_SSBO);
    glDeleteVertexArrays(1, &m_VAO);
}

void ParticleSystem::Update(Shader& Shader, GLfloat DeltaTime, Entity& entity)
{
    Shader.Use();
    Shader.setFloat("deltaTime", DeltaTime);
    Shader.setVec3("gravity", 0.0f, 1.0f, 0.0f);
    glDispatchCompute(glm::max(m_Amount / 1024, 1U), 1, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
}

// Render all particles
void ParticleSystem::Draw(Shader& shader)
{
    shader.Use();
    glDisable(GL_CULL_FACE);
    glBindVertexArray(m_VAO);
    glEnable(GL_PROGRAM_POINT_SIZE);

    glDrawArrays(GL_POINTS, 0, m_Amount);

    glDisable(GL_PROGRAM_POINT_SIZE);
    glBindVertexArray(0);
    glEnable(GL_CULL_FACE);
}

void ParticleSystem::init()
{
    m_Positions.reserve(m_Amount);
    m_Velocities.reserve(m_Amount);
    m_Lifetimes.reserve(m_Amount);
    for (int i = 0; i < m_Amount; i++)
    {
        GLfloat randomX = ((rand() % 50) - 25) / 14.0f;
        GLfloat randomY = ((rand() % 50)) / 10.0f;
        GLfloat randomZ = ((rand() % 50) - 25) / 14.0f;
        GLfloat lifeTime = (rand() % (int)(m_MaxLifeTime * 10)) / 10.0f;
        // initialize particle data
        m_Positions.push_back(glm::vec4(randomX, randomY, randomZ, 1.0f));
        m_Velocities.push_back(glm::vec4(1.0f));
        m_Lifetimes.push_back(glm::vec2(0.0f, lifeTime));
    }

    glGenBuffers(3, m_SSBO);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_SSBO[0]);
    glBufferData(GL_SHADER_STORAGE_BUFFER, m_Amount * sizeof(glm::vec4), &m_Positions[0], GL_DYNAMIC_DRAW);

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, m_SSBO[1]);
    glBufferData(GL_SHADER_STORAGE_BUFFER, m_Amount * sizeof(glm::vec4), &m_Velocities[0], GL_DYNAMIC_DRAW);

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, m_SSBO[2]);
    glBufferData(GL_SHADER_STORAGE_BUFFER, m_Amount * sizeof(glm::vec2), &m_Lifetimes[0], GL_DYNAMIC_DRAW);

    glCreateVertexArrays(1, &m_VAO);

    glVertexArrayVertexBuffer(m_VAO, 0, m_SSBO[0], 0, sizeof(glm::vec4));
    glEnableVertexArrayAttrib(m_VAO, 0);
    glVertexArrayAttribFormat(m_VAO, 0, 4, GL_FLOAT, GL_FALSE, 0);
    glVertexArrayAttribBinding(m_VAO, 0, 0);

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    glBindVertexArray(0);

}