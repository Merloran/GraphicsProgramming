#include "Public/Animation.h"
#include "Public/SkinnedModel.h"
#include "Public/BoneInfo.h"
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <glm/gtx/matrix_decompose.hpp>

Animation::Animation(const std::string& animationPath, SkinnedModel* model)
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(animationPath, aiProcess_Triangulate);
	assert(scene && scene->mRootNode);
	aiAnimation* animation = scene->mAnimations[0];
	m_Duration = animation->mDuration;
	m_TicksPerSecond = animation->mTicksPerSecond;
	//std::cout << animation->mDuration << " " << animation->mTicksPerSecond << std::endl;
	//aiMatrix4x4 globalTransformation = scene->mRootNode->mTransformation;
	//globalTransformation = globalTransformation.Inverse();
	ReadHierarchyData(m_RootNode, scene->mRootNode);
	ReadMissingBones(animation, *model);
	//for (auto& c : m_Bones)
	//{
	//	std::cout << c.GetBoneName() << std::endl;
	//	for (auto& p : c.m_Positions)
	//	{
	//		std::cout << p.position[0] << " " << p.position[1] << " " << p.position[2] << std::endl;
	//		std::cout << p.timeStamp << std::endl;
	//	}
	//	for (auto& p : c.m_Rotations)
	//	{
	//		std::cout << p.orientation[0] << " " << p.orientation[1] << " " << p.orientation[2] << " " << p.orientation[3] << std::endl;
	//		std::cout << p.timeStamp << std::endl;
	//	}
	//}
}

Bone* Animation::FindBone(const std::string& name)
{
	auto iter = std::find_if(m_Bones.begin(), m_Bones.end(),
		[&](const Bone& Bone)
		{
			return Bone.GetBoneName() == name;
		}
	);

	if (iter == m_Bones.end())
	{
		return nullptr;
	}
	else
	{
		return &(*iter);
	}
}

float Animation::GetTicksPerSecond()
{
	return m_TicksPerSecond;
}

float Animation::GetDuration()
{
	return m_Duration;
}

const AssimpNodeData& Animation::GetRootNode()
{ 
	return m_RootNode;
}

const std::unordered_map<std::string, BoneInfo>& Animation::GetBoneIDMap()
{ 
	return m_BoneInfoMap;
}

void Animation::ReadMissingBones(const aiAnimation* animation, SkinnedModel& model)
{
	int32_t size = animation->mNumChannels;

	std::unordered_map<std::string, BoneInfo>& modelBoneInfoMap = model.GetBoneInfoMap();//getting m_BoneInfoMap from Model class
	int32_t boneCount = model.GetBoneCount(); //getting the m_BoneCounter from Model class

	//reading channels(bones engaged in an animation and their keyframes)
	for (int32_t i = 0; i < size; i++)
	{
		aiNodeAnim* channel = animation->mChannels[i];
		std::string boneName = channel->mNodeName.data;

		//if (modelBoneInfoMap.find(boneName) == modelBoneInfoMap.end())
		//{
		//	modelBoneInfoMap[boneName].ID = boneCount;
		//	boneCount++;
		//}
		m_Bones.push_back(Bone(channel->mNodeName.data, modelBoneInfoMap[channel->mNodeName.data].ID, channel));
	}
	//for (auto& b : m_Bones)
	//{
	//	std::cout << b.GetBoneName() << " " << b.GetBoneID() << std::endl;
	//}

	m_BoneInfoMap = modelBoneInfoMap;
}

void Animation::ReadHierarchyData(AssimpNodeData& dest, const aiNode* src)
{
	assert(src);
	aiVector3D pos;
	aiQuaternion rot;
	src->mTransformation.DecomposeNoScaling(rot, pos);

	//bool is_node_bone = pos.x || pos.y || pos.z;

	//if (!is_node_bone) {
	//	for (int32_t i = 0; i < src->mNumChildren; i++) {
	//		ReadHierarchyData(dest, src->mChildren[i]);
	//	}
	//	return;
	//}

	glm::vec3 glmPos = AnimationOptimizer::GetGLMVec(pos);
	glm::quat glmRot = AnimationOptimizer::GetGLMQuat(rot);
	dest.position = glmPos;
	dest.rotation = glmRot;
	//std::cout << src->mName.C_Str() << std::endl;
	//std::cout << glmPos[0] << " " << glmPos[1] << " " << glmPos[2] << std::endl;
	//std::cout << glmRot[0] << " " << glmRot[1] << " " << glmRot[2] << " " << glmRot[3] << std::endl;
	//uint16_t posComp[3];
	//AnimationOptimizer::Vec4Com16bit(glm::vec4(glmPos, 0.0f), posComp);
	//uint16_t rotComp[3];
	//AnimationOptimizer::QuatFhm16bit(glmRot, rotComp);
	//dest.position[0] = posComp[0];
	//dest.position[1] = posComp[1];
	//dest.position[2] = posComp[2];

	//dest.rotation[0] = rotComp[0];
	//dest.rotation[1] = rotComp[1];
	//dest.rotation[2] = rotComp[2];

	dest.name = src->mName.data;

	dest.children.reserve(src->mNumChildren);

	for (int32_t i = 0; i < src->mNumChildren; i++)
	{
		AssimpNodeData newData;
		ReadHierarchyData(newData, src->mChildren[i]);
		dest.children.push_back(newData);
	}
}
