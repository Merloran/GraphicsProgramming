#pragma once
#include "glm/glm.hpp"

class Shader;

class ColliderAABB
{
public:
	// Center of collider
	glm::vec3 Center;
	// Distances between center and faces in 3 directions
	glm::vec3 Range;

	ColliderAABB(glm::vec3 center, glm::vec3 range);
	ColliderAABB();

	void Draw(Shader& shader);
};

