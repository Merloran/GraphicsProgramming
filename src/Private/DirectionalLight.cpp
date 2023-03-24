#include "Public/DirectionalLight.h"

#include <glm/gtc/type_ptr.hpp>
#include "Public/Line.h"
#include "Public/Circle.h"

DirectionalLight::DirectionalLight(const glm::vec3& Direction, const glm::vec3& Color)
	: Light(Color)
	, m_Direction(Direction)
{
	if (m_IDCounter > MAX_LIGHT_NUMBER)
	{
		fprintf(stderr, "Max directional light number exceeded.\n");
		return;
	}

	m_ID = m_IDCounter++;
	m_Prefix = "dirLights[" + std::to_string(m_ID) + ']';
}

void DirectionalLight::SetDirection(const glm::vec3& Direction)
{
	m_Direction = Direction;
}

void DirectionalLight::SetupShader(Shader& Shader)
{
	Shader.Use();
	Shader.setBool((m_Prefix + ".isOn").c_str(), m_IsOn);
	Shader.setVec3((m_Prefix + ".direction").c_str(), m_Direction);
	Shader.setVec3((m_Prefix + ".color").c_str(), m_Color * m_Intensity);
}

void DirectionalLight::Draw(Shader& Shader)
{
	if (!isGizmosOn)
	{
		return;
	}
	Shader.Use();
	Shader.setMat4("model", glm::mat4(1.0f));
	Circle circle(0.5f, -m_Direction);
	circle.Draw(Shader);
	glm::vec3 pos = glm::vec3(circle.vertices[0], circle.vertices[1], circle.vertices[2]);
	Line line(pos, pos + m_Direction * 0.2f);
	line.Draw(Shader);
	pos = glm::vec3(circle.vertices[15], circle.vertices[16], circle.vertices[17]);
	line = Line(pos, pos + m_Direction * 0.2f);
	line.Draw(Shader);
	pos = glm::vec3(circle.vertices[30], circle.vertices[31], circle.vertices[32]);
	line = Line(pos, pos + m_Direction * 0.2f);
	line.Draw(Shader);
	pos = glm::vec3(circle.vertices[45], circle.vertices[46], circle.vertices[47]);
	line = Line(pos, pos + m_Direction * 0.2f);
	line.Draw(Shader);
}

const glm::vec3& DirectionalLight::GetDirection() const
{
	return m_Direction;
}

void DirectionalLight::Print()
{
	fprintf(stdout, "ID: %d\n", m_ID);

	fprintf(stdout, "Direction: "); PrintVec(m_Direction);

	fprintf(stdout, "Color: "); PrintVec(m_Color);
}
