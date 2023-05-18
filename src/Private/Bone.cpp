#include "Public/Bone.h"
#include <iostream>

Bone::Bone(const std::string& name, int32_t ID, const aiNodeAnim* channel)
	: m_Name(name)
	, m_ID(ID)
	, m_LocalTransform(1.0f)
{
	m_NumPositions = channel->mNumPositionKeys;

	//std::cout << channel->mNumPositionKeys << " " << channel->mNumRotationKeys << " " << channel->mNumScalingKeys << std::endl;
	for (int32_t positionIndex = 0; positionIndex < m_NumPositions; ++positionIndex)
	{
		aiVector3D aiPosition = channel->mPositionKeys[positionIndex].mValue;
		float timeStamp = channel->mPositionKeys[positionIndex].mTime;
		KeyPosition data;

		data.position.x = aiPosition.x;
		data.position.y = aiPosition.y;
		data.position.z = aiPosition.z;

		data.timeStamp = timeStamp;
		m_Positions.push_back(data);
	}

	m_NumRotations = channel->mNumRotationKeys;
	for (int32_t rotationIndex = 0; rotationIndex < m_NumRotations; ++rotationIndex)
	{
		aiQuaternion aiOrientation = channel->mRotationKeys[rotationIndex].mValue;
		float timeStamp = channel->mRotationKeys[rotationIndex].mTime;
		KeyRotation data;

		data.orientation.x = aiOrientation.x;
		data.orientation.y = aiOrientation.y;
		data.orientation.z = aiOrientation.z;
		data.orientation.w = aiOrientation.w;

		data.timeStamp = timeStamp;
		m_Rotations.push_back(data);
	}

	//for (int i = 0; i < m_Positions.size(); ++i)
	//{
	//	std::cout << "Pos: " << m_Positions[i].position[0] << " " << m_Positions[i].position[1] << " " << m_Positions[i].position[2] << std::endl;
	//	std::cout << "Rot: " << m_Rotations[i].orientation[0] << " " << m_Rotations[i].orientation[1] << " " << m_Rotations[i].orientation[2] << " " << m_Rotations[i].orientation[3] << std::endl;
	//}
}

void Bone::Update(float animationTime)
{
	glm::mat4 translation = InterpolatePosition(animationTime);
	glm::mat4 rotation = InterpolateRotation(animationTime);
	//std::cout << m_Name << std::endl;
	//for (int i = 0; i < 4; ++i)
	//{
	//	for (int j = 0; j < 4; ++j)
	//		std::cout << translation[i][j] << " ";
	//	std::cout << std::endl;
	//}
	//std::cout << std::endl;
	//for (int i = 0; i < 4; ++i)
	//{
	//	for (int j = 0; j < 4; ++j)
	//		std::cout << rotation[i][j] << " ";
	//	std::cout << std::endl;
	//}
	//std::cout << std::endl;
	//std::cout << std::endl;

	m_LocalTransform = translation * rotation;
}

glm::mat4 Bone::GetLocalTransform()
{ 
	return m_LocalTransform;
}

std::string Bone::GetBoneName() const
{ 
	return m_Name; 
}

int32_t Bone::GetBoneID()
{ 
	return m_ID;
}

int32_t Bone::GetPositionIndex(float animationTime)
{
	for (int32_t index = 0; index < m_NumPositions - 1; ++index)
	{
		if (animationTime < m_Positions[index + 1].timeStamp)
		{
			return index;
		}
	}
	assert(0);
}

int32_t Bone::GetRotationIndex(float animationTime)
{
	for (int32_t index = 0; index < m_NumRotations - 1; ++index)
	{
		if (animationTime < m_Rotations[index + 1].timeStamp)
		{
			return index;
		}
	}
	assert(0);
}


float Bone::GetScaleFactor(float lastTimeStamp, float nextTimeStamp, float animationTime)
{
	float midWayLength = animationTime - lastTimeStamp;
	float framesDiff = nextTimeStamp - lastTimeStamp;
	float scaleFactor = midWayLength / framesDiff;
	return scaleFactor;
}

glm::mat4 Bone::InterpolatePosition(float animationTime)
{
	if (m_NumPositions == 1)
	{
		return glm::translate(glm::mat4(1.0f), m_Positions[0].position);
	}

	int32_t p0Index = GetPositionIndex(animationTime);
	int32_t p1Index = p0Index + 1;
	float scaleFactor = GetScaleFactor(m_Positions[p0Index].timeStamp, m_Positions[p1Index].timeStamp, animationTime);
	glm::vec3 finalPosition = glm::mix(m_Positions[p0Index].position, m_Positions[p1Index].position, scaleFactor);
	return glm::translate(glm::mat4(1.0f), finalPosition);
}

glm::mat4 Bone::InterpolateRotation(float animationTime)
{
	if (m_NumRotations == 1)
	{
		glm::quat rotation = glm::normalize(m_Rotations[0].orientation);
		return glm::toMat4(rotation);
	}

	int32_t p0Index = GetRotationIndex(animationTime);
	int32_t p1Index = p0Index + 1;
	float scaleFactor = GetScaleFactor(m_Rotations[p0Index].timeStamp, m_Rotations[p1Index].timeStamp, animationTime);
	glm::quat finalRotation = glm::slerp(m_Rotations[p0Index].orientation, m_Rotations[p1Index].orientation, scaleFactor);
	finalRotation = glm::normalize(finalRotation);
	return glm::toMat4(finalRotation);
}