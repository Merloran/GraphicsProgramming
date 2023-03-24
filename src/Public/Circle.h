#pragma once

#include "Object.h"

class Circle : public Object
{
public:
    Circle() = default;
    Circle(float Radius, glm::vec3 Position);

    void Draw(Shader& Shader) override;

    ~Circle();
    float vertices[63];
private:
    unsigned int m_VAO, m_VBO;
};
