#include "Public/BloomRenderer.h"
#include <iostream>
#include "Public/Quad.h"

BloomRenderer::BloomRenderer(unsigned int WindowWidth, unsigned int WindowHeight)
	: m_Init(false)
{
	this->Init(WindowWidth, WindowHeight);
}

BloomRenderer::~BloomRenderer()
{
	for (int i = 0; i < (int)m_MipChain.size(); i++)
	{
		glDeleteTextures(1, &m_MipChain[i].texture);
		m_MipChain[i].texture = 0;
	}
	glDeleteFramebuffers(1, &m_FBO);
	m_FBO = 0;
	delete m_DownsampleShader;
	delete m_UpsampleShader;
	m_Init = false;
}

bool BloomRenderer::Init(unsigned int WindowWidth, unsigned int WindowsHeight)
{

	if (m_Init) return true;

	glGenFramebuffers(1, &m_FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);

	glm::vec2 mipSize((float)WindowWidth, (float)WindowsHeight);
	glm::ivec2 mipIntSize((int)WindowWidth, (int)WindowsHeight);
	// Safety check
	if (WindowWidth > (unsigned int)INT_MAX || WindowsHeight > (unsigned int)INT_MAX) 
	{
		std::cerr << "Window size conversion overflow - cannot build bloom FBO!" << std::endl;
		return false;
	}

	for (GLuint i = 0; i < 6; i++)
	{
		bloomMip mip;

		mipSize *= 0.5f;
		mipIntSize /= 2;
		mip.size = mipSize;
		mip.intSize = mipIntSize;

		glGenTextures(1, &mip.texture);
		glBindTexture(GL_TEXTURE_2D, mip.texture);
		// we are downscaling an HDR color buffer, so we need a float texture format
		glTexImage2D(GL_TEXTURE_2D, 0, GL_R11F_G11F_B10F, (int)mipSize.x, (int)mipSize.y, 0, GL_RGB, GL_FLOAT, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		//std::cout << "Created bloom mip " << mipIntSize.x << 'x' << mipIntSize.y << std::endl;
		m_MipChain.emplace_back(mip);
	}

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_MipChain[0].texture, 0);

	// setup attachments
	unsigned int attachments[1] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, attachments);

	// check completion status
	int status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE)
	{
		printf("gbuffer FBO error, status: 0x%x\n", status);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		return false;
	}


	m_SrcViewportSize = glm::ivec2(WindowWidth, WindowsHeight);
	m_SrcViewportSizeFloat = glm::vec2((float)WindowWidth, (float)WindowsHeight);


	// Shaders
	m_DownsampleShader = new Shader("res/shaders/Sample.vert", "res/shaders/DownSample.frag");
	m_UpsampleShader = new Shader("res/shaders/Sample.vert", "res/shaders/UpSample.frag");

	// Downsample
	m_DownsampleShader->Use();
	m_DownsampleShader->setInt("srcTexture", 0);

	// Upsample
	m_UpsampleShader->Use();
	m_UpsampleShader->setInt("srcTexture", 0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	m_Init = true;
	return true;
}


void BloomRenderer::RenderDownsamples(unsigned int srcTexture)
{
	m_DownsampleShader->Use();
	m_DownsampleShader->setVec2("srcResolution", m_SrcViewportSizeFloat);
	if (m_KarisAverageOnDownsample) 
	{
		m_DownsampleShader->setInt("mipLevel", 0);
	}

	// Bind srcTexture (HDR color buffer) as initial texture input
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, srcTexture);

	// Progressively downsample through the mip chain
	for (int i = 0; i < (int)m_MipChain.size(); i++)
	{
		const bloomMip& mip = m_MipChain[i];
		glViewport(0, 0, mip.size.x, mip.size.y);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mip.texture, 0);

		// Render screen-filled quad of resolution of current mip
		Quad::GetInstance().Draw(*m_DownsampleShader);

		// Set current mip resolution as srcResolution for next iteration
		m_DownsampleShader->setVec2("srcResolution", mip.size);
		// Set current mip as texture input for next iteration
		glBindTexture(GL_TEXTURE_2D, mip.texture);
		// Disable Karis average for consequent downsamples
		if (i == 0) 
		{
			m_DownsampleShader->setInt("mipLevel", 1); 
		}
	}
}
void BloomRenderer::RenderUpsamples(float filterRadius)
{
	m_UpsampleShader->Use();
	m_UpsampleShader->setFloat("filterRadius", filterRadius);

	// Enable additive blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);
	glBlendEquation(GL_FUNC_ADD);

	for (int i = (int)m_MipChain.size() - 1; i > 0; i--)
	{
		const bloomMip& mip = m_MipChain[i];
		const bloomMip& nextMip = m_MipChain[i - 1];

		// Bind viewport and texture from where to read
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, mip.texture);

		// Set framebuffer render target (we write to this texture)
		glViewport(0, 0, nextMip.size.x, nextMip.size.y);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
			GL_TEXTURE_2D, nextMip.texture, 0);

		// Render screen-filled quad of resolution of current mip
		Quad::GetInstance().Draw(*m_UpsampleShader);
	}

	// Disable additive blending
	//glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_BLEND);

	glUseProgram(0);
}

void BloomRenderer::RenderBloomTexture(unsigned int srcTexture, float filterRadius)
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);

	this->RenderDownsamples(srcTexture);
	this->RenderUpsamples(filterRadius);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	// Restore viewport
	glViewport(0, 0, m_SrcViewportSize.x, m_SrcViewportSize.y);
}

GLuint BloomRenderer::BloomTexture()
{
	return m_MipChain[0].texture;
}
