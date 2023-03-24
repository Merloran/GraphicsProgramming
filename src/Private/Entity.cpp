#include "Public/Entity.h"
#include "imgui.h"
#include "imgui_impl/imgui_impl_glfw.h"
#include "imgui_impl/imgui_impl_opengl3.h"
#include <iostream>
#include "Public/PointLight.h"
#include "Public/SpotLight.h"
#include "Public/DirectionalLight.h"

Entity::Entity(Object& Object, const std::string& Name, Shader& DefaultShader)
	: object(&Object)
	, parent(nullptr)
	, defaultShader(&DefaultShader)
	, name(Name)
	, m_ID(m_IDCounter++)
	, m_IsRefract(false)
	, transform(glm::vec3(0.0f), glm::vec3(0.0f), glm::vec3(1.0f))
{
}

Entity::Entity(const std::string& Name)
	: parent(nullptr)
	, object(nullptr)
	, defaultShader(nullptr)
	, name(Name)
	, transform(glm::vec3(0.0f), glm::vec3(0.0f), glm::vec3(1.0f))
	, m_ID(m_IDCounter++)
	, m_IsRefract(false)
{
}

void Entity::AddChild(Object& Object, const std::string& Name, Shader& DefaultShader)
{
	children.emplace_back(std::make_shared<Entity>(Object, Name, DefaultShader));
	children.back()->parent = this;
}

void Entity::UpdateSelfAndChildren()
{
	if (!transform.IsDirty())
	{
		return;
	}

	ForceUpdateSelfAndChildren();
}

void Entity::ForceUpdateSelfAndChildren()
{
	if (parent)
	{
		transform.CalculateModel(parent->transform.GetModel());
	}
	else
	{
		transform.CalculateModel();
	}

	for (std::shared_ptr<Entity>& child : children)
	{
		child->ForceUpdateSelfAndChildren();
	}
}

void Entity::DrawSelfAndChildren(Shader& Shader)
{
	if (object)
	{
		Shader.setMat4("model", transform.GetModel());
		Shader.setBool("isRefract", m_IsRefract);
		object->Draw(Shader);
	}

	for (std::shared_ptr<Entity>& child : children)
	{
		child->DrawSelfAndChildren(Shader);
	}
}

void Entity::DrawSelfAndChildren()
{
	if (object)
	{
		defaultShader->Use();
		defaultShader->setMat4("model", transform.GetModel());
		defaultShader->setBool("isRefract", m_IsRefract);
		object->Draw(*defaultShader);
	}

	for (std::shared_ptr<Entity>& child : children)
	{
		child->DrawSelfAndChildren();
	}
}

void Entity::DrawGUITree()
{
	ImGuiTreeNodeFlags flags = (this == m_SelectedEntity ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;
	bool isOpen = ImGui::TreeNodeEx(name.c_str(), flags);

	if (ImGui::IsItemClicked())
	{
		m_SelectedEntity = this;
	}
	if (isOpen)
	{
		for (std::shared_ptr<Entity>& child : children)
		{
			child->DrawGUITree();
		}
		ImGui::TreePop();
	}
}

Entity* Entity::FindByName(std::string Name)
{
	if (name == Name)
	{
		return this;
	}
	for (std::shared_ptr<Entity>& child : children)
	{
		if (child.get()->FindByName(Name) != nullptr)
		{
			return child.get();
		}
		
	}
	return nullptr;
}

void Entity::DrawGUIEdit()
{
	SpotLight* spot = dynamic_cast<SpotLight*>(object);
	DirectionalLight* dir = dynamic_cast<DirectionalLight*>(object);
	PointLight* point = dynamic_cast<PointLight*>(object);

	if (spot)
	{
		glm::vec3 Color = spot->GetColor();
		ImGui::ColorEdit3("Color", &Color[0]);
		spot->SetColor(Color);

		float Intensity = spot->GetIntensity();
		ImGui::SliderFloat("Intensity", &Intensity, 0.0f, 5000.0f);
		spot->SetIntensity(Intensity);

		glm::vec3 Position = spot->GetPosition();
		ImGui::InputFloat3("Position", &Position[0]);
		spot->SetPosition(Position);

		glm::vec3 Direction = spot->GetDirection();
		ImGui::InputFloat3("Direction", &Direction[0]);
		spot->SetDirection(Direction);

		float CutOff = spot->GetCutOff();
		ImGui::SliderFloat("Angle", &CutOff, 0.0f, 180.0f);
		spot->SetCutOff(CutOff);

		float Outer = spot->GetOuter();
		ImGui::SliderFloat("Outer", &Outer, 0.0f, 180.0f);
		spot->SetOuter(Outer);
	}
	else if (dir)
	{
		glm::vec3 Color = dir->GetColor();
		ImGui::ColorEdit3("Color", &Color[0]);
		dir->SetColor(Color);

		float Intensity = dir->GetIntensity();
		ImGui::SliderFloat("Intensity", &Intensity, 0.0f, 5000.0f);
		dir->SetIntensity(Intensity);

		glm::vec3 Direction = dir->GetDirection();
		ImGui::InputFloat3("Direction", &Direction[0]);
		dir->SetDirection(Direction);
	}
	else if (point)
	{
		glm::vec3 Color = point->GetColor();
		ImGui::ColorEdit3("Color", &Color[0]);
		point->SetColor(Color);

		float Intensity = point->GetIntensity();
		ImGui::SliderFloat("Intensity", &Intensity, 0.0f, 5000.0f);
		point->SetIntensity(Intensity);

		glm::vec3 Position = point->GetPosition();
		ImGui::InputFloat3("Position", &Position[0]);
		point->SetPosition(Position);
	}
	else
	{
		glm::vec3 Position = transform.GetLocalPosition();
		ImGui::InputFloat3("Position", &Position[0]);
		transform.SetLocalPosition(Position);

		glm::vec3 Rotation = transform.GetLocalRotation();
		ImGui::InputFloat3("Rotation", &Rotation[0]);
		transform.SetLocalRotation(Rotation);

		glm::vec3 Scale = transform.GetLocalScale();
		ImGui::InputFloat3("Scale", &Scale[0]);
		transform.SetLocalScale(Scale);

		ImGui::Checkbox("isRefract", &m_IsRefract);
	}
}

Entity* Entity::GetSelectedEntity()
{
	return m_SelectedEntity;
}

unsigned int Entity::GetID() const
{
	return m_ID;
}

bool Entity::operator==(const Entity& Other)
{
	return Other.GetID() == this->GetID();
}