#include "Public/PBRManager.h"

#include <glm/gtc/matrix_transform.hpp>

#include "Public/Shader.h"
#include "Public/Texture.h"
#include "Public/CubeMap.h"
#include "Public/Cube.h"
#include "Public/Quad.h"

PBRManager::PBRManager()
{
    SetMatrixes();

    glGenFramebuffers(1, &m_FBO);
    glGenRenderbuffers(1, &m_RBO);

    glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);
    glBindRenderbuffer(GL_RENDERBUFFER, m_RBO);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_RBO);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

PBRManager& PBRManager::GetInstance()
{
    static PBRManager instance;
    return instance;
}

PBRManager::~PBRManager()
{
    glDeleteFramebuffers(1, &m_FBO);
    glDeleteRenderbuffers(1, &m_RBO);
}

void PBRManager::SetupEquirectangular(Shader& Shader, Texture& HDRMap, CubeMap& EnvironmentMap)
{
    glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);
    glBindRenderbuffer(GL_RENDERBUFFER, m_RBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, EnvironmentMap.GetWidth(), EnvironmentMap.GetHeight());

    Shader.Use();
    HDRMap.BindTexture(0);
    Shader.setInt("equirectangularMap", 0);
    Shader.setMat4("projection", m_Projection);
    
    glViewport(0, 0, EnvironmentMap.GetWidth(), EnvironmentMap.GetHeight());
    for (unsigned int i = 0; i < 6; ++i)
    {
        Shader.setMat4("view", m_Views[i]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, EnvironmentMap.GetId(), 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        Cube::GetInstance().Draw(Shader);
    }

    // then let OpenGL generate mipmaps from first mip face (combatting visible dots artifact)
    glBindTexture(GL_TEXTURE_CUBE_MAP, EnvironmentMap.GetId());
    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

void PBRManager::SetupIrradiance(Shader& Shader, CubeMap& IrradianceMap, CubeMap& EnvironmentMap)
{
    glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);
    glBindRenderbuffer(GL_RENDERBUFFER, m_RBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, IrradianceMap.GetWidth(), IrradianceMap.GetHeight());

    // pbr: solve diffuse integral by convolution to create an irradiance (cube)map.
    // -----------------------------------------------------------------------------
    Shader.Use();
    EnvironmentMap.BindCubeMap(0);
    Shader.setInt("environmentMap", 0);
    Shader.setMat4("projection", m_Projection);

    glViewport(0, 0, IrradianceMap.GetWidth(), IrradianceMap.GetHeight());
    for (unsigned int i = 0; i < 6; ++i)
    {
        Shader.setMat4("view", m_Views[i]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, IrradianceMap.GetId(), 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        Cube::GetInstance().Draw(Shader);
    }

    glBindVertexArray(0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

void PBRManager::SetupPrefilter(Shader& Shader, CubeMap& PrefilterMap, CubeMap& EnvironmentMap, unsigned int MaxMipLevels)
{
    glBindTexture(GL_TEXTURE_CUBE_MAP, PrefilterMap.GetId());
    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

    // pbr: run a quasi monte-carlo simulation on the environment lighting to create a prefilter (cube)map.
    // ----------------------------------------------------------------------------------------------------
    Shader.Use();
    EnvironmentMap.BindCubeMap(0);
    Shader.setInt("environmentMap", 0);
    Shader.setMat4("projection", m_Projection);

    glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);
    for (unsigned int mip = 0; mip < MaxMipLevels; ++mip)
    {
        // reisze framebuffer according to mip-level size.
        unsigned int mipWidth = static_cast<unsigned int>(PrefilterMap.GetWidth() * std::pow(0.5, mip));
        unsigned int mipHeight = static_cast<unsigned int>(PrefilterMap.GetHeight() * std::pow(0.5, mip));
        glBindRenderbuffer(GL_RENDERBUFFER, m_RBO);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mipWidth, mipHeight);
        glViewport(0, 0, mipWidth, mipHeight);

        float roughness = (float)mip / (float)(MaxMipLevels - 1);
        Shader.setFloat("roughness", roughness);
        for (unsigned int i = 0; i < 6; ++i)
        {
            Shader.setMat4("view", m_Views[i]);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, PrefilterMap.GetId(), mip);

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            Cube::GetInstance().Draw(Shader);
        }
    }
    glBindVertexArray(0);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

void PBRManager::SetupBRDF(Shader& Shader, Texture& BRDFMap)
{   
    // then re-configure capture framebuffer object and render screen-space quad with BRDF shader.
    glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);
    glBindRenderbuffer(GL_RENDERBUFFER, m_RBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, BRDFMap.GetWidth(), BRDFMap.GetHeight());
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, BRDFMap.GetId(), 0);

    glViewport(0, 0, BRDFMap.GetWidth(), BRDFMap.GetHeight());
    Shader.Use();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    Quad::GetInstance().Draw(Shader);

    glBindVertexArray(0);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void PBRManager::SetMatrixes()
{
    m_Projection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
    m_Views[0]   = glm::lookAt(glm::vec3(0.0f), glm::vec3( 1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f));
    m_Views[1]   = glm::lookAt(glm::vec3(0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f));
    m_Views[2]   = glm::lookAt(glm::vec3(0.0f), glm::vec3( 0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f));
    m_Views[3]   = glm::lookAt(glm::vec3(0.0f), glm::vec3( 0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f));
    m_Views[4]   = glm::lookAt(glm::vec3(0.0f), glm::vec3( 0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f));
    m_Views[5]   = glm::lookAt(glm::vec3(0.0f), glm::vec3( 0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f));
}
