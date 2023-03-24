#pragma once

#include "Light.h"

class DirectionalLight : public Light
{
public:
	DirectionalLight(const glm::vec3& Direction, const glm::vec3& Color);

	void SetDirection(const glm::vec3& Direction);

	void SetupShader(Shader& Shader) override;

	void Draw(Shader& Shader) override;

	const glm::vec3& GetDirection() const;

	void Print();

private:
	glm::vec3 m_Direction;

	const unsigned int MAX_LIGHT_NUMBER = 1U;

	static inline unsigned int m_IDCounter = 0U;
};

