#pragma once

#include "Object.h"

class Quad : public Object
{
public:
    Quad(Quad const&) = delete;
    void operator=(Quad const&) = delete;

    static Quad& GetInstance();

    void Draw(Shader& Shader) override;

    ~Quad();

private:
    unsigned int m_VAO, m_VBO;
    Quad();
};

