#include "Public/Transform.h"

#include <glm/gtc/matrix_transform.hpp>


Transform::Transform(glm::vec3 Position, glm::vec3 Rotation, glm::vec3 Scale)
	: m_Position(Position)
	, m_Rotation(Rotation)
	, m_Scale(Scale)
	, m_IsDirty(true)
{
	CalculateModel();
}

void Transform::CalculateModel()
{
	m_Model = GetLocalModel();
}

void Transform::CalculateModel(const glm::mat4& ParentGlobalModel)
{
	m_Model = ParentGlobalModel * GetLocalModel();
}

void Transform::SetLocalPosition(const glm::vec3& Position)
{
	m_Position = Position;
	m_IsDirty = true;
}

void Transform::SetLocalRotation(const glm::vec3& Rotation)
{
	m_Rotation = Rotation;
	m_IsDirty = true;
}

void Transform::SetLocalScale(const glm::vec3& Scale)
{
	m_Scale = Scale;
	m_IsDirty = true;
}

const glm::vec3& Transform::GetGlobalPosition() const
{
	return m_Model[3];
}

const glm::vec3& Transform::GetLocalPosition() const
{
	return m_Position;
}

const glm::vec3& Transform::GetLocalRotation() const
{
	return m_Rotation;
}

const glm::vec3& Transform::GetLocalScale() const
{
	return m_Scale;
}

glm::vec3 Transform::GetGlobalScale() const
{
	return glm::vec3(glm::length(GetRight()), glm::length(GetUp()), glm::length(GetForward()));
}

const glm::mat4& Transform::GetModel() const
{
	return m_Model;
}

glm::vec3 Transform::GetRight() const
{
	return m_Model[0];
}

glm::vec3 Transform::GetUp() const
{
	return m_Model[1];
}

glm::vec3 Transform::GetForward() const
{
	return m_Model[2];
}

bool Transform::IsDirty() const
{
	return m_IsDirty;
}

void Transform::PrintModel()
{
	for (int j = 0; j < 4; ++j)
	{
		for (int i = 0; i < 4; ++i)
		{
			fprintf(stdout, "%f ", m_Model[i][j]);
		}
		fprintf(stdout, "\n");
	}
}


glm::mat4 Transform::GetLocalModel()
{

	const glm::mat4 transformX = glm::rotate(glm::mat4(1.0f), glm::radians(m_Rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
	const glm::mat4 transformY = glm::rotate(glm::mat4(1.0f), glm::radians(m_Rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
	const glm::mat4 transformZ = glm::rotate(glm::mat4(1.0f), glm::radians(m_Rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));

	// Y * X * Z
	const glm::mat4 roationMatrix = transformY * transformX * transformZ;

	// translation * rotation * scale (also know as TRS matrix)
	return glm::translate(glm::mat4(1.0f), m_Position) * roationMatrix * glm::scale(glm::mat4(1.0f), m_Scale);
}