#pragma once
#include <vector>
#include <glm/glm.hpp>
#include "Public/Bone.h"
#include <string>
#include <map>

class aiAnimation;
class aiNode;
class BoneInfo;
class SkinnedModel;

struct AssimpNodeData
{
	glm::mat4 transformation;
	std::string name;
	int32_t childrenCount;
	std::vector<AssimpNodeData> children;
};

class Animation
{
public:
	Animation() = default;

	Animation(const std::string& animationPath, SkinnedModel* model);

	~Animation() = default;

	Bone* FindBone(const std::string& name);

	float GetTicksPerSecond();
	float GetDuration();
	const AssimpNodeData& GetRootNode();
	const std::map<std::string, BoneInfo>& GetBoneIDMap();

private:
	void ReadMissingBones(const aiAnimation* animation, SkinnedModel& model);

	void ReadHierarchyData(AssimpNodeData& dest, const aiNode* src);
	float m_Duration;
	int m_TicksPerSecond;
	std::vector<Bone> m_Bones;
	AssimpNodeData m_RootNode;
	std::map<std::string, BoneInfo> m_BoneInfoMap;
};

