#pragma once

#include "DirectionalLight.h"

class Shader;
class Entity;

class Shadow
{
public:
	Shadow(int Width, int Height, float Near = 1.0f, float Far = 500.5f);

	~Shadow();

	void SetupMap(Shader& Shader, DirectionalLight& Light, Entity& Root);
	// Bind shadow map to specified texture
	void BindShadowMap(unsigned int Number);

	void SetNearPlane(float Value);
	void SetFarPlane(float Value);

	unsigned int GetMap() const;
	const glm::mat4& GetLightSpace() const;


private:
	unsigned int m_FBO;
	unsigned int m_MAP;
	float m_Near, m_Far;
	glm::mat4 m_Projection;
	glm::mat4 m_LightSpace;

	const int WIDTH, HEIGHT;
};

