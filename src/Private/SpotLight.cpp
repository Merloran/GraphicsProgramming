#include "Public/SpotLight.h"
#include "Public/Line.h"

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>

SpotLight::SpotLight(glm::vec3 Position, glm::vec3 Direction, glm::vec3 Color)
	: PointLight(Color)
	, m_Direction(glm::normalize(Direction))
{
	if (m_IDCounter > MAX_LIGHT_NUMBER)
	{
		fprintf(stderr, "Max spot light number exceeded.\n");
		return;
	}

	m_Position = Position;
	SetAttenuationParams();

	m_LightAngle = 12.5f;
	m_Outer = 5.0f;

	m_ID = m_IDCounter++;
	m_Prefix = "spotLights[" + std::to_string(m_ID) + ']';
}

void SpotLight::SetDirection(glm::vec3 Direction)
{
	m_Direction = glm::normalize(Direction);
}

void SpotLight::SetCutOff(float CutOff)
{
	m_LightAngle = CutOff;
}

void SpotLight::SetOuter(float Outer)
{
	m_Outer = Outer;
}

glm::vec3 SpotLight::GetDirection()
{
	return m_Direction;
}

float SpotLight::GetCutOff()
{
	return m_LightAngle;
}

float SpotLight::GetOuter()
{
	return m_Outer;
}

void SpotLight::SetupShader(Shader& Shader)
{
	Shader.Use();
	Shader.setBool((m_Prefix + ".isOn").c_str(), m_IsOn);
	Shader.setVec3((m_Prefix + ".position").c_str(), m_Position);
	Shader.setVec3((m_Prefix + ".direction").c_str(), m_Direction);
	Shader.setVec3((m_Prefix + ".color").c_str(), m_Color * m_Intensity);
	Shader.setFloat((m_Prefix + ".cutOff").c_str(), glm::cos(glm::radians(m_LightAngle)));
	Shader.setFloat((m_Prefix + ".outerCutOff").c_str(), glm::cos(glm::radians(m_LightAngle + m_Outer)));
	Shader.setFloat((m_Prefix + ".constant").c_str(), m_Constant);
	Shader.setFloat((m_Prefix + ".linear").c_str(), m_Linear);
	Shader.setFloat((m_Prefix + ".quadratic").c_str(), m_Quadratic);
}

void SpotLight::Draw(Shader& Shader)
{
	if (!isGizmosOn)
	{
		return;
	}
	Shader.Use();
	float Radius = glm::length(m_Direction * m_Radius) * tan(glm::radians(m_LightAngle + m_Outer));
	m_Gizmo = Circle(Radius, glm::vec3(0.0f));
	glm::mat4 model(1.0f);
	glm::vec3 forward(0.0f, 0.0f, 1.0f);
	glm::mat4 RotationMatrix(1.0f);
	if (forward != m_Direction)
	{
		glm::vec3 helper = glm::normalize(glm::cross(forward, m_Direction));
		float angle = acos(glm::dot(forward, m_Direction));
		glm::quat Q = glm::quat(cos(angle / 2.0f), sin(angle / 2.0f) * helper);
		RotationMatrix = glm::toMat4(Q);
		model = glm::translate(glm::mat4(1.0f), m_Position + m_Direction * m_Radius) * RotationMatrix;
	}
	else
	{
		model = glm::translate(glm::mat4(1.0f), m_Position + m_Direction * m_Radius);
	}
	Shader.setMat4("model", model);

	Line line(glm::vec3(0.0f, 0.0f, -1.0f) * m_Radius, glm::vec3(m_Gizmo.vertices[0], m_Gizmo.vertices[1], m_Gizmo.vertices[2]));
	line.Draw(Shader);
	line = Line(glm::vec3(0.0f, 0.0f, -1.0f) * m_Radius, glm::vec3(m_Gizmo.vertices[15], m_Gizmo.vertices[16], m_Gizmo.vertices[17]));
	line.Draw(Shader);
	line = Line(glm::vec3(0.0f, 0.0f, -1.0f) * m_Radius, glm::vec3(m_Gizmo.vertices[30], m_Gizmo.vertices[31], m_Gizmo.vertices[32]));
	line.Draw(Shader);
	line = Line(glm::vec3(0.0f, 0.0f, -1.0f) * m_Radius, glm::vec3(m_Gizmo.vertices[45], m_Gizmo.vertices[46], m_Gizmo.vertices[47]));
	line.Draw(Shader);
	m_Gizmo.Draw(Shader);
}

void SpotLight::Print()
{
	fprintf(stdout, "ID: %d\n", m_ID);

	fprintf(stdout, "Position: "); PrintVec(m_Position);

	fprintf(stdout, "Direction: "); PrintVec(m_Direction);

	fprintf(stdout, "Color: "); PrintVec(m_Color);

	fprintf(stdout, "CutOff: %f\n", m_LightAngle);

	fprintf(stdout, "Outer: %f\n", m_Outer);

	fprintf(stdout, "Constant: %f\n", m_Constant);

	fprintf(stdout, "Linear: %f\n", m_Linear);

	fprintf(stdout, "Quadratic: %f\n", m_Quadratic);
}
