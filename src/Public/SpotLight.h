#pragma once

#include "PointLight.h"

class SpotLight : public PointLight
{
public:
	SpotLight(glm::vec3 Position, glm::vec3 Direction, glm::vec3 Color);


	void SetDirection(glm::vec3 Direction);
	void SetCutOff(float LightAngle);
	void SetOuter(float Outer);

	glm::vec3 GetDirection();
	float GetCutOff();
	float GetOuter();

	virtual void SetupShader(Shader& Shader) override;
	virtual void Draw(Shader& Shader) override;

	void Print();

private:
	glm::vec3 m_Direction;

	float m_LightAngle;
	float m_Outer;

	const unsigned int MAX_LIGHT_NUMBER = 2U;
	static inline unsigned int m_IDCounter = 0U;
};

