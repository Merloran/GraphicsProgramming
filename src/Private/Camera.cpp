#include "../Public/Camera.h"

#include <GLFW/glfw3.h>

Camera::Camera(float Speed, float Sensitivity)
    : Speed(Speed)
    , Position(glm::vec3(0.0f))
    , Rotation(glm::vec3(0.0f))
    , ForwardVector(glm::vec3(1.0f, 0.0f, 0.0f))
    , UpVector(glm::vec3(0.0f, 1.0f, 0.0f))
    , RightVector(glm::vec3(0.0f, 0.0f, 1.0f))
    , WorldUp(glm::vec3(0.0f, 1.0f, 0.0f))
    , m_LastX(0.0)
    , m_LastY(0.0)
    , m_Sensitivity(Sensitivity)
    , m_FirstActivation(true)
{}


void Camera::GetKeyboardInput(GLFWwindow* window, Move Direction, GLfloat deltaTime)
{
    switch (Direction)
    {
    case Move::FORWARD:
    {
        Position += (Speed * ForwardVector) * deltaTime;
        break;
    }
    case Move::BACKWARD:
    {
        Position -= (Speed * ForwardVector) * deltaTime;
        break;
    }
    case Move::LEFT:
    {
        Position -= (Speed * RightVector) * deltaTime;
        break;
    }
    case Move::RIGHT:
    {
        Position += (Speed * RightVector) * deltaTime;
        break;
    }
    case Move::UP:
    {
        Position += (Speed * WorldUp) * deltaTime;
        break;
    }
    case Move::DOWN:
    {
        Position -= (Speed * WorldUp) * deltaTime;
        break;
    }
    default:
        break;
    }
}

void Camera::GetMouseInput(GLFWwindow* window)
{
    double x, y;
    glfwGetCursorPos(window, &x, &y);
    if (m_FirstActivation)
    {
        m_LastX = x;
        m_LastY = y;
        m_FirstActivation = false;
    }

    GLfloat xoffset = x - m_LastX;
    GLfloat yoffset = m_LastY - y;
    m_LastX = x;
    m_LastY = y;

    xoffset *= m_Sensitivity;
    yoffset *= m_Sensitivity;

    Rotation.x += yoffset;
    Rotation.y += xoffset;
}

void Camera::UpdateForwardVector()
{
    if (Rotation.x > 89.0f)
    {
        Rotation.x = 89.0f;
    }
    else if (Rotation.x < -89.0f)
    {
        Rotation.x = -89.0f;
    }
    if (Rotation.y >= 360.f)
    {
        Rotation.y = 0.0f;
    }
    else if (Rotation.y < 0.0)
    {
        Rotation.y = 360.0f - FLT_MIN;
    }

    ForwardVector.x = cos(glm::radians(Rotation.y)) * cos(glm::radians(Rotation.x));
    ForwardVector.y = sin(glm::radians(Rotation.x));
    ForwardVector.z = sin(glm::radians(Rotation.y)) * cos(glm::radians(Rotation.x));
    ForwardVector = glm::normalize(ForwardVector);

    RightVector = glm::normalize(glm::cross(ForwardVector, glm::vec3(0.0f, 1.0f, 0.0f)));
    UpVector = glm::normalize(glm::cross(RightVector, ForwardVector));
}

glm::mat4 Camera::LookAt(glm::vec3 position, glm::vec3 target)
{
    glm::vec3 Forward = glm::normalize(position - target);
    glm::vec3 Right = glm::normalize(glm::cross(glm::normalize(WorldUp), Forward));
    glm::vec3 Up = glm::cross(Forward, Right);

    glm::mat4 VectorMatrix(Right.x, Up.x, Forward.x, 0.0f,
                           Right.y, Up.y, Forward.y, 0.0f,
                           Right.z, Up.z, Forward.z, 0.0f,
                           0.0f,    0.0f, 0.0f,      1.0f);

    glm::mat4 PositionMatrix( 1.0f,        0.0f,        0.0f,       0.0f,
                              0.0f,        1.0f,        0.0f,       0.0f,
                              0.0f,        0.0f,        1.0f,       0.0f,
                             -Position.x, -Position.y, -Position.z, 1.0f);

    return VectorMatrix * PositionMatrix;
}

void Camera::GetMouseScroll(GLFWwindow* window, double xoffset, double yoffset)
{
    if (Zoom < 1.0f)
    {
        Zoom = 1.0f;
    }
    else if (Zoom > 45.0f)
    {
        Zoom = 45.0f;
    }
    else
    {
        Zoom -= yoffset;
    }
}
