#pragma once
#include "glm/glm.hpp"
#include "glad/glad.h"
#include <vector>
#include "Shader.h"

// bloom stuff
struct bloomMip
{
	glm::vec2 size;
	glm::ivec2 intSize;
	unsigned int texture;
};

class BloomRenderer
{
public:
	BloomRenderer(unsigned int WindowWidth, unsigned int WindowHeight);
	~BloomRenderer();
	bool Init(unsigned int WindowWidth, unsigned int WindowHeight);
	void RenderBloomTexture(unsigned int srcTexture, float filterRadius);
	unsigned int BloomTexture();

private:
	void RenderDownsamples(unsigned int srcTexture);
	void RenderUpsamples(float filterRadius);
	std::vector<bloomMip> m_MipChain;
	unsigned int m_FBO;
	bool m_Init;
	glm::ivec2 m_SrcViewportSize;
	glm::vec2 m_SrcViewportSizeFloat;
	Shader* m_DownsampleShader;
	Shader* m_UpsampleShader;

	bool m_KarisAverageOnDownsample = true;
};