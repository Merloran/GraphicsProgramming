#pragma once

#include <glm/glm.hpp>

#include "Light.h"
#include "Circle.h"


enum class AttenuationDist : char
{
	D_7,
	D_13,
	D_20,
	D_32,
	D_50,
	D_65,
	D_100,
	D_160,
	D_200,
	D_325,
	D_600,
	D_3250,
	Default = D_50
};

class Shader;

class PointLight : public Light
{
public:
	PointLight(glm::vec3 Position, glm::vec3 Color, AttenuationDist Distance = AttenuationDist::Default);

	void SetPosition(glm::vec3 Position);

	void SetAttenuationParams(float Constant, float Linear, float Quadratic);
	void SetAttenuationParams(AttenuationDist Distance = AttenuationDist::Default);

	virtual void SetupShader(Shader& Shader) override;

	virtual void Draw(Shader& Shader) override;

	const glm::vec3& GetPosition() const;

	void Print();

protected:
	PointLight(glm::vec3 Color);
	glm::vec3 m_Position;

	float m_Constant;
	float m_Linear;
	float m_Quadratic;
	float m_Radius;
	Circle m_Gizmo;

private:
	const unsigned int MAX_LIGHT_NUMBER = 3U;
	static inline unsigned int m_IDCounter = 0U;
};

