#pragma once

class ColliderAABB;

class CollisionManager
{
public:
    CollisionManager(CollisionManager const&) = delete;
    void operator=(CollisionManager const&) = delete;

    static CollisionManager& GetInstance();

	bool Intersects(ColliderAABB& A, ColliderAABB& B);

    void HandleCollision(ColliderAABB& A, ColliderAABB& B);
private:
    CollisionManager();
};

