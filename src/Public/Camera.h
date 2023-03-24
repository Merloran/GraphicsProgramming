#pragma once

#include <glm/glm.hpp>
#include <glad/glad.h> 

enum class Move
{
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT,
	UP,
	DOWN
};

struct GLFWwindow;

class Camera
{
public:
	Camera(float Speed = 5.0f, float m_Sensitivity = 0.1f);

	void GetKeyboardInput(GLFWwindow* window, Move Direction, GLfloat deltaTime);

	void GetMouseInput(GLFWwindow* window);

	// Not implemented
	void GetMouseScroll(GLFWwindow* window, double xoffset, double yoffset);

	void UpdateForwardVector();

	glm::mat4 LookAt(glm::vec3 position, glm::vec3 target);

	// Not implemented
	float Zoom;

	float Speed;

	glm::vec3 Position;
	glm::vec3 Rotation;

	glm::vec3 ForwardVector;
	glm::vec3 UpVector;
	glm::vec3 RightVector;

	glm::vec3 WorldUp;

private:
	double m_LastX, m_LastY;
	float m_Sensitivity;
	bool m_FirstActivation;
};

