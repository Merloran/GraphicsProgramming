#include "Public/ColliderAABB.h"
#include "Public/Shader.h"
#include "Public/Cube.h"
#include "glm/gtc/matrix_transform.hpp"
#include <iostream>

ColliderAABB::ColliderAABB(glm::vec3 center, glm::vec3 range)
	: Center(center)
	, Range(range)
{
}

ColliderAABB::ColliderAABB()
	: Center(0.0f)
	, Range(0.0f)
{
}

void ColliderAABB::Draw(Shader& shader)
{
	shader.Use();

	glm::mat4 model = glm::translate(glm::mat4(1.0f), Center);
	model = glm::scale(model, Range);
	shader.setMat4("model", model);

	Cube::GetInstance().Draw(shader);
}
