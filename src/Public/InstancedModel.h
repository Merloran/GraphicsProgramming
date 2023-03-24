#pragma once
#include <vector>

#include "Model.h"
#include "Transform.h"

class InstancedModel : public Model
{
public:
	InstancedModel(const char* Path, std::vector<glm::mat4> Transforms);
	~InstancedModel();

	void Draw(Shader& Shader) override;
private:
	unsigned int m_InstanceVBO;
	int m_ElementsCount;
};

