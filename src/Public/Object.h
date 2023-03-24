#pragma once
#include "Shader.h"

class Object
{
public:
	virtual void Draw(Shader& shader) = 0;
};