#pragma once

#include <string>
#include <glm/glm.hpp>

#include "Object.h"
#include "Shader.h"

class Light : public Object
{
public:
	Light(glm::vec3 Color);

	virtual void SetupShader(Shader& Shader) = 0;

	void SetColor(glm::vec3 Color);
	glm::vec3 GetColor();
	void SetIntensity(float Intensity);
	float GetIntensity();
	void SetIsOn(bool IsOn);
	unsigned int GetID() const;

	static inline bool isGizmosOn = false;
protected:
	unsigned int m_ID;
	std::string m_Prefix;

	bool m_IsOn;
	glm::vec3 m_Color;
	float m_Intensity;

	void PrintVec(glm::vec3 V);
};

