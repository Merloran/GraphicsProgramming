#pragma once

#include <glm/glm.hpp>

class Transform
{
public:
	Transform(glm::vec3 Position, glm::vec3 Rotation, glm::vec3 Scale);

	void CalculateModel();
	void CalculateModel(const glm::mat4& ParentGlobalModel);

	void SetLocalPosition(const glm::vec3& Position);
	void SetLocalRotation(const glm::vec3& Rotation);
	void SetLocalScale(const glm::vec3& Scale);

	const glm::vec3& GetGlobalPosition() const;
	const glm::vec3& GetLocalPosition() const;
	const glm::vec3& GetLocalRotation() const;
	const glm::vec3& GetLocalScale() const;
	glm::vec3 GetGlobalScale() const;
	const glm::mat4& GetModel() const;

	glm::vec3 GetRight() const;
	glm::vec3 GetUp() const;
	glm::vec3 GetForward() const;

	bool IsDirty() const;

	void PrintModel();

protected:
	glm::mat4 GetLocalModel();

private:
	glm::vec3 m_Position;
	glm::vec3 m_Rotation;
	glm::vec3 m_Scale;

	glm::mat4 m_Model;

	bool m_IsDirty;
};

