#include "Public/Shadow.h"

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Public/Entity.h"

Shadow::Shadow(int Width, int Height, float Near, float Far)
	: WIDTH(Width)
	, HEIGHT(Height)
    , m_Near(Near)
    , m_Far(Far)
{
    m_Projection = glm::ortho(-50.0f, 50.0f, -50.0f, 50.0f, m_Near, m_Far);
    glGenFramebuffers(1, &m_FBO);
    glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);

    glGenTextures(1, &m_MAP);
    glBindTexture(GL_TEXTURE_2D, m_MAP);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, WIDTH, HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

    float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

    // Bind CBO to FBO
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_MAP, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    // Bind defaults
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

Shadow::~Shadow()
{
    glDeleteFramebuffers(1, &m_FBO);
    glDeleteTextures(1, &m_MAP);
}

void Shadow::SetupMap(Shader& Shader, DirectionalLight& Light, Entity& Root)
{
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::mat4 lightView = glm::lookAt(-Light.GetDirection(), glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
    m_LightSpace = m_Projection * lightView;

    // render scene from light's point of view
    Shader.Use();
    Shader.setMat4("lightSpaceMatrix", m_LightSpace);

    glViewport(0, 0, WIDTH, HEIGHT);
    glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);
    glClear(GL_DEPTH_BUFFER_BIT);

    Root.DrawSelfAndChildren(Shader);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Shadow::BindShadowMap(unsigned int Number)
{
    if (Number > 31)
    {
        fprintf(stderr, "Failed to bind texture to %d", Number);
        return;
    }
    else
    {
        glActiveTexture(GL_TEXTURE0 + Number);
        glBindTexture(GL_TEXTURE_2D, m_MAP);
        glActiveTexture(GL_TEXTURE0);
    }
}

void Shadow::SetNearPlane(float Value)
{
    m_Near = Value;
    m_Projection = glm::ortho(-50.0f, 50.0f, -50.0f, 50.0f, m_Near, m_Far);
}

void Shadow::SetFarPlane(float Value)
{
    m_Far = Value;
    m_Projection = glm::ortho(-50.0f, 50.0f, -50.0f, 50.0f, m_Near, m_Far);
}

unsigned int Shadow::GetMap() const
{
    return m_MAP;
}

const glm::mat4& Shadow::GetLightSpace() const
{
    return m_LightSpace;
}
