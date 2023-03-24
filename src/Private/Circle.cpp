#include "Public/Circle.h"
#define _USE_MATH_DEFINES
#include <math.h>

void Circle::Draw(Shader& Shader)
{
    Shader.Use();
    glBindVertexArray(m_VAO);
    glDrawArrays(GL_LINE_STRIP, 0, 21);
    glBindVertexArray(0);
}

Circle::~Circle()
{
	//glDeleteVertexArrays(1, &m_VAO);
	//glDeleteBuffers(1, &m_VBO);
}

Circle::Circle(float Radius, glm::vec3 Position)
{
	const int VerticesNum = 21;
	const float TwoPI = 2 * M_PI;
	for (int i = 0; i < VerticesNum; ++i)
	{
		vertices[i * 3] = Position.x + Radius * cos(i * TwoPI / (VerticesNum - 1));
		vertices[i * 3 + 1] = Position.y + Radius * sin(i * TwoPI / (VerticesNum - 1));
		vertices[i * 3 + 2] = Position.z;
	}

	glGenVertexArrays(1, &m_VAO);
	glGenBuffers(1, &m_VBO);
	// fill buffer
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	// link vertex attributes
	glBindVertexArray(m_VAO);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}
