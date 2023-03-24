#include "Public/Light.h"

Light::Light(glm::vec3 Color)
	: m_Color(Color)
	, m_Intensity(1.0f)
	, m_IsOn(true)
{
}

void Light::SetColor(glm::vec3 Color)
{
	m_Color = Color;
}

glm::vec3 Light::GetColor()
{
	return m_Color;
}

void Light::SetIntensity(float Intensity)
{
	m_Intensity = Intensity;
}

float Light::GetIntensity()
{
	return m_Intensity;
}

void Light::SetIsOn(bool IsOn)
{
	m_IsOn = IsOn;
}

unsigned int Light::GetID() const
{
	return m_ID;
}

void Light::PrintVec(glm::vec3 V)
{
	fprintf(stdout, "(%f, %f, %f)\n", V.x, V.y, V.z);
}