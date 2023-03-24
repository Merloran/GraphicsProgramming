#include "Public/Line.h"

Line::Line(glm::vec3 Start, glm::vec3 End)
{
	vertices[0] = Start[0];
	vertices[1] = Start[1];
	vertices[2] = Start[2];
	vertices[3] = End[0];
	vertices[4] = End[1];
	vertices[5] = End[2];

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

void Line::Draw(Shader& Shader)
{
    Shader.Use();
    glBindVertexArray(m_VAO);
    glDrawArrays(GL_LINE_STRIP, 0, 2);
    glBindVertexArray(0);
}
