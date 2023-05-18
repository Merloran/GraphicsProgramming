#include "Public/Animator.h"
#include "Public/Animation.h"
#include "Public/Bone.h"
#include <iostream>

Animator::Animator(Animation* animation)
{
	m_CurrentTime = 0.0f;
	m_CurrentAnimation = animation;

	m_FinalBoneMatrices.reserve(512);

	for (int32_t i = 0; i < 512; ++i)
	{
		m_FinalBoneMatrices.push_back(glm::mat4(1.0f));
	}
	//std::cout << m_CurrentAnimation->GetRootNode().name << std::endl;
	//for (int32_t i = 0; i < m_CurrentAnimation->GetRootNode().children.size(); ++i)
	//{
	//	std::cout << m_CurrentAnimation->GetRootNode().children[i].name << std::endl;
	//}
	//for (int32_t i = 0; i < m_CurrentAnimation->GetRootNode().children[0].children.size(); ++i)
	//{
	//	std::cout << m_CurrentAnimation->GetRootNode().children[0].children[i].name << std::endl;
	//}
}

void Animator::UpdateAnimation(float dt)
{
	m_DeltaTime = dt;
	if (m_CurrentAnimation)
	{
		m_CurrentTime += m_CurrentAnimation->GetTicksPerSecond() * dt;
		m_CurrentTime = fmod(m_CurrentTime, m_CurrentAnimation->GetDuration());
		CalculateBoneTransform(&m_CurrentAnimation->GetRootNode(), glm::mat4(1.0f));
	}
}

void Animator::PlayAnimation(Animation* pAnimation)
{
	m_CurrentAnimation = pAnimation;
	m_CurrentTime = 0.0f;
}

void Animator::CalculateBoneTransform(const AssimpNodeData* node, const glm::mat4& parentTransform)
{
	std::string nodeName = node->name;
	//glm::vec3 posDecomp = AnimationOptimizer::Vec4Decom16bit(node->position);
	//glm::quat rotDecomp = AnimationOptimizer::QuatIhm16bit(node->rotation);
	glm::vec3 posDecomp = node->position;
	glm::quat rotDecomp = node->rotation;
	//std::cout << nodeName << std::endl;
	glm::mat4 nodeTransform = glm::translate(glm::mat4(1.0f), posDecomp);
	nodeTransform *= glm::toMat4(rotDecomp);

	Bone* Bone = m_CurrentAnimation->FindBone(nodeName);

	if (Bone)
	{
		Bone->Update(m_CurrentTime);
		nodeTransform = Bone->GetLocalTransform();
		//std::cout << nodeName << std::endl;
		//for (int i = 0; i < 4; ++i)
		//{
		//	for (int j = 0; j < 4; ++j)
		//		std::cout << nodeTransform[i][j] << " ";
		//	std::cout << std::endl;
		//}
		//std::cout << std::endl;
	}

	glm::mat4 globalTransformation = parentTransform * nodeTransform;

	std::unordered_map<std::string, BoneInfo> boneInfoMap = m_CurrentAnimation->GetBoneIDMap();
	if (boneInfoMap.find(nodeName) != boneInfoMap.end())
	{
		int32_t index = boneInfoMap[nodeName].ID;

		glm::mat4 offset = glm::translate(glm::mat4(1.0f), boneInfoMap[nodeName].Position);
		offset *= glm::toMat4(boneInfoMap[nodeName].Rotation);

		m_FinalBoneMatrices[index] = globalTransformation * offset;
	}
	
	for (int32_t i = 0; i < node->children.size(); ++i)
	{
		CalculateBoneTransform(&node->children[i], globalTransformation);
	}
}

std::vector<glm::mat4> Animator::GetFinalBoneMatrices()
{
	return m_FinalBoneMatrices;
}
