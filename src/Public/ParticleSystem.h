#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <glad/glad.h>
#include "Entity.h"
#include "Shader.h"
#include "Texture.h"

class ParticleSystem
{
public:
    ParticleSystem(Texture& texture, GLuint amount, float maxLifeTime);

    ~ParticleSystem();

    // Update all particles
    void Update(Shader& Shader, GLfloat DeltaTime, Entity& entity);
    // Render all particles
    void Draw(Shader& shader);
private:

    GLuint m_SSBO[4];
    std::vector<glm::vec4> m_Positions;
    std::vector<glm::vec4> m_Velocities;
    std::vector<glm::vec2> m_Lifetimes;
    float m_MaxLifeTime;
    GLuint m_Amount;
    Texture* m_Texture;
    GLuint m_VAO;
    void init();
};