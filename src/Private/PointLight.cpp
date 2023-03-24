#include "Public/PointLight.h"

#include <glm/gtc/type_ptr.hpp>

PointLight::PointLight(glm::vec3 Position, glm::vec3 Color, AttenuationDist Distance)
	: m_Position(Position)
	, Light(Color)
{
	if (m_IDCounter > MAX_LIGHT_NUMBER)
	{
		fprintf(stderr, "Max point light number exceeded.\n");
		return;
	}

	SetAttenuationParams(Distance);
	m_ID = m_IDCounter++;
	m_Prefix = "pointLights[" + std::to_string(m_ID) + ']';
}

void PointLight::SetPosition(glm::vec3 Position)
{
	m_Position = Position;
}

void PointLight::SetAttenuationParams(float Constant, float Linear, float Quadratic)
{
	m_Constant = Constant;
	m_Linear = Linear;
	m_Quadratic = Quadratic;

	float Delta = m_Linear * m_Linear + 316.0f * m_Quadratic;
	m_Radius = -(m_Linear + sqrt(Delta)) / (2 * m_Quadratic);
	m_Gizmo = Circle(m_Radius, glm::vec3(0.0f));
}

void PointLight::SetAttenuationParams(AttenuationDist Distance)
{
	m_Constant = 1.0f;
	switch (Distance)
	{
		case AttenuationDist::D_7:
		{
			m_Linear = 0.7f;
			m_Quadratic = 1.8f;
			break;
		}
		case AttenuationDist::D_13:
		{
			m_Linear = 0.35f;
			m_Quadratic = 0.44f;
			break;
		}
		case AttenuationDist::D_20:
		{
			m_Linear = 0.22f;
			m_Quadratic = 0.20f;
			break;
		}
		case AttenuationDist::D_32:
		{
			m_Linear = 0.14f;
			m_Quadratic = 0.07f;
			break;
		}
		case AttenuationDist::D_65:
		{
			m_Linear = 0.07f;
			m_Quadratic = 0.017f;
			break;
		}
		case AttenuationDist::D_100:
		{
			m_Linear = 0.045f;
			m_Quadratic = 0.0075f;
			break;
		}
		case AttenuationDist::D_160:
		{
			m_Linear = 0.027f;
			m_Quadratic = 0.0028f;
			break;
		}
		case AttenuationDist::D_200:
		{
			m_Linear = 0.022f;
			m_Quadratic = 0.0019f;
			break;
		}
		case AttenuationDist::D_325:
		{
			m_Linear = 0.014f;
			m_Quadratic = 0.0007f;
			break;
		}
		case AttenuationDist::D_600:
		{
			m_Linear = 0.007f;
			m_Quadratic = 0.0002f;
			break;
		}
		case AttenuationDist::D_3250:
		{
			m_Linear = 0.0014f;
			m_Quadratic = 0.000007f;
			break;
		}
		case AttenuationDist::D_50:
		default:
		{
			m_Linear = 0.09f;
			m_Quadratic = 0.032f;
			break;
		}
	}

	float Delta = m_Linear * m_Linear + 316.0f * m_Quadratic;
	m_Radius = (-m_Linear + sqrt(Delta)) / (2 * m_Quadratic);
	m_Gizmo = Circle(m_Radius, glm::vec3(0.0f));
}

void PointLight::SetupShader(Shader& Shader)
{
	Shader.Use();
	Shader.setBool((m_Prefix + ".isOn").c_str(), m_IsOn);
	Shader.setVec3((m_Prefix + ".position").c_str(), m_Position);
	Shader.setVec3((m_Prefix + ".color").c_str(), m_Color * m_Intensity);
	Shader.setFloat((m_Prefix + ".constant").c_str(), m_Constant);
	Shader.setFloat((m_Prefix + ".linear").c_str(), m_Linear);
	Shader.setFloat((m_Prefix + ".quadratic").c_str(), m_Quadratic);
}

void PointLight::Draw(Shader& Shader)
{
	if (!isGizmosOn)
	{
		return;
	}

	Shader.Use();

	glm::mat4 model(1.0f);
	model = glm::translate(model, m_Position);
	Shader.setMat4("model", model);
	m_Gizmo.Draw(Shader);

	model = glm::translate(glm::mat4(1.0f), m_Position);
	model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	Shader.setMat4("model", model);
	m_Gizmo.Draw(Shader);

	model = glm::translate(glm::mat4(1.0f), m_Position);
	model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	Shader.setMat4("model", model);
	m_Gizmo.Draw(Shader);
}

const glm::vec3& PointLight::GetPosition() const
{
	return m_Position;
}

void PointLight::Print()
{
	fprintf(stdout, "ID: %d\n", m_ID);

	fprintf(stdout, "Position: "); PrintVec(m_Position);

	fprintf(stdout, "Color: "); PrintVec(m_Color);

	fprintf(stdout, "Constant: %f\n", m_Constant);

	fprintf(stdout, "Linear: %f\n", m_Linear);

	fprintf(stdout, "Quadratic: %f\n", m_Quadratic);
}

PointLight::PointLight(glm::vec3 Color)
	: Light(Color)
{
}
