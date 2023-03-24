#pragma once

#include <list>
#include <memory>
#include <string>
#include "Transform.h"
#include "Object.h"


class Entity
{
public:
    //Scene graph
    std::list<std::shared_ptr<Entity>> children;
    Entity* parent;
    Object* object;
    Shader* defaultShader;
    std::string name;

    //Space information
    Transform transform;

    Entity(Object& Object, const std::string& Name, Shader& DefaultShader);
    Entity(const std::string& Name = "Root");

    void AddChild(Object& Object, const std::string& Name, Shader& DefaultShader);

    void UpdateSelfAndChildren();
    void ForceUpdateSelfAndChildren();
    void DrawSelfAndChildren(Shader& Shader);
    void DrawSelfAndChildren();
    void DrawGUITree();
    void DrawGUIEdit();
    static Entity* GetSelectedEntity();
    Entity* FindByName(std::string Name);

    unsigned int GetID() const;

    bool operator==(const Entity& Other);

private:
    bool m_IsRefract;
    inline static Entity* m_SelectedEntity = nullptr;
    inline static unsigned int m_IDCounter = 0u;
    unsigned int m_ID;
};

