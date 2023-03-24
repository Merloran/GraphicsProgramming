#pragma once

#include <glm/glm.hpp>

class Texture;
class CubeMap;
class Shader;

class PBRManager
{
public:
	PBRManager(PBRManager const&) = delete;
	void operator=(PBRManager const&) = delete;

	static PBRManager& GetInstance();
	~PBRManager();

	void SetupEquirectangular(Shader& Shader, Texture& HDRMap, CubeMap& EnvironmentMap);
	void SetupIrradiance(Shader& Shader, CubeMap& IrradianceMap, CubeMap& EnvironmentMap);
	void SetupPrefilter(Shader& Shader, CubeMap& PrefilterMap, CubeMap& EnvironmentMap, unsigned int MaxMipLevels = 5);
	void SetupBRDF(Shader& Shader, Texture& BRDFMap);

private:
	void SetMatrixes();
	PBRManager();

	glm::mat4 m_Projection;
	glm::mat4 m_Views[6];
	unsigned int m_FBO, m_RBO;
};

