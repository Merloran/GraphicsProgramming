#pragma once

#include "Object.h"

class Cube : public Object
{
public:
    Cube(Cube const&) = delete;
    void operator=(Cube const&) = delete;

    static Cube& GetInstance();

    void Draw(Shader& Shader) override;

    ~Cube();

private:
    unsigned int m_VAO, m_VBO;
    Cube();
};
