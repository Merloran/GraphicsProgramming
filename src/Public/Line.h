#pragma once

#include "Object.h"

class Line : public Object
{
public:
    Line() = default;
    Line(glm::vec3 Start, glm::vec3 End);

    void Draw(Shader& Shader) override;

    ~Line();
    float vertices[6];
private:
    unsigned int m_VAO, m_VBO;
};