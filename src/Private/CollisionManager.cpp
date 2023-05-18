#include "Public/CollisionManager.h"
#include "Public/ColliderAABB.h"
#include "glm/glm.hpp"
#include <iostream>

CollisionManager& CollisionManager::GetInstance()
{
    static CollisionManager instance;
    return instance;
}

bool CollisionManager::Intersects(ColliderAABB& A, ColliderAABB& B)
{
    // Check X axis
    if (abs(A.Center.x - B.Center.x) >= (A.Range.x + B.Range.x))
    {
        return false;
    }

    // Check Y axis
    if (abs(A.Center.y - B.Center.y) >= (A.Range.y + B.Range.y))
    {
        return false;
    }

    // Check Z axis
    if (abs(A.Center.z - B.Center.z) >= (A.Range.z + B.Range.z))
    {
        return false;
    }

    return true;
}

void CollisionManager::HandleCollision(ColliderAABB& A, ColliderAABB& B)
{
    if (Intersects(A, B))
    {
        const float l1 = A.Center.x - A.Range.x, l2 = B.Center.x - B.Range.x;
        const float r1 = A.Center.x + A.Range.x, r2 = B.Center.x + B.Range.x;
        const float b1 = A.Center.z - A.Range.z, b2 = B.Center.z - B.Range.z;
        const float f1 = A.Center.z + A.Range.z, f2 = B.Center.z + B.Range.z;
        const float d1 = A.Center.y - A.Range.y, d2 = B.Center.y - B.Range.y;
        const float u1 = A.Center.y + A.Range.y, u2 = B.Center.y + B.Range.y;

        const float left  = r1 - l2, right = r2 - l1;
        const float up    = u1 - d2, down  = u2 - d1;
        const float front = f1 - b2, back  = f2 - b1;

        glm::vec3 Direction(0.0f);

        left  < right ? Direction.x = -left  : Direction.x = right;
        up    < down  ? Direction.y = -up    : Direction.y = down;
        front < back  ? Direction.z = -front : Direction.z = back;

        Direction.x * Direction.x < Direction.y * Direction.y ? Direction.y = 0.0f : Direction.x = 0.0f;

        if (Direction.x == 0.0f)
        {
            Direction.z * Direction.z < Direction.y * Direction.y ? Direction.y = 0.0f : Direction.z = 0.0f;
        }
        else
        {
            Direction.z * Direction.z < Direction.x * Direction.x ? Direction.x = 0.0f : Direction.z = 0.0f;
        }

        const float SlideSpeed = 4.0f / 5.0f;
        A.Center += Direction * SlideSpeed;
        B.Center -= Direction * (1.0f - SlideSpeed);
    }
}

CollisionManager::CollisionManager()
{
}
