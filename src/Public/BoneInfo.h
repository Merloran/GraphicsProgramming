#pragma once

#include<assimp/quaternion.h>
#include<assimp/vector3.h>
#include<assimp/matrix4x4.h>
#include<glm/glm.hpp>
#include<glm/gtc/quaternion.hpp>

struct BoneInfo
{
	/*id is index in finalBoneMatrices*/
	int32_t ID;

	/*offset matrix transforms vertex from model space to bone space*/
	//glm::mat4 Offset;
	glm::vec3 Position;
	glm::quat Rotation;

};

class AnimationOptimizer
{
public:


	static inline glm::vec3 GetGLMVec(const aiVector3D& vec)
	{
		return glm::vec3(vec.x, vec.y, vec.z);
	}

	static inline glm::quat GetGLMQuat(const aiQuaternion& pOrientation)
	{
		return glm::quat(pOrientation.w, pOrientation.x, pOrientation.y, pOrientation.z);
	}

	static uint16_t* QuatFhm16bit(const glm::quat& q)
	{
		glm::vec3 vec = QuatFhm(q);
		return Vec3To16bit(vec);
	}

	static glm::quat QuatIhm16bit(const uint16_t* b)
	{
		glm::vec3 vec = I6bitToVec3(b);
		return QuatIhm(vec);
	}

private:
	// 4(sqrt(2)-1)
	static inline const float Km = 4.0f * 0.4142135679721832275390625f;
	// sqrt(2)+1 = 1/(sqrt(2)-1)
	static inline const float Khf = 2.414213657379150390625f;
	// 3-2sqrt(2)
	static inline const float Khi = 0.17157287895679473876953125f;

	static inline const float PosRange = 20.0f;

	// compression
	static glm::vec3 QuatFhm(const glm::quat& q)
	{
		glm::vec3 v;

		float s = Khf / (1.0f + q.w + sqrt(2.0f + 2.0f * q.w));

		v.x = q.x * s;
		v.y = q.y * s;
		v.z = q.z * s;

		return v;
	}

	// decompression
	static glm::quat QuatIhm(const glm::vec3& v)
	{
		float d = Khi * glm::dot(v, v);
		float a = (1.0f + d);
		float b = (1.0f - d) * Km;
		float c = 1.0f / (a * a);

		float bc = b * c;

		glm::quat q;
		q.x = v.x * bc;
		q.y = v.y * bc;
		q.z = v.z * bc;
		q.w = (1.0f + d * (d - 6.0f)) * c;

		return q;
	}

	static inline float DecompressFloatMinusOnePlusOne(uint16_t Value)
	{
		return (float(Value) / 65535.0f) * 2.0f - 1.0f;
	}

	static inline uint16_t CompressFloatMinusOnePlusOne(float Value)
	{
		return uint16_t(((Value + 1.0f) / 2.0f) * 65535.0f);
	}

	static uint16_t* Vec3To16bit(const glm::vec3& v)
	{
		uint16_t result[3];

		result[0] = CompressFloatMinusOnePlusOne(v.x);
		result[1] = CompressFloatMinusOnePlusOne(v.y);
		result[2] = CompressFloatMinusOnePlusOne(v.z);

		return result;
	}

	static glm::vec3 I6bitToVec3(const uint16_t* b)
	{
		glm::vec3 result;

		result.x = CompressFloatMinusOnePlusOne(b[0]);
		result.y = CompressFloatMinusOnePlusOne(b[1]);
		result.z = CompressFloatMinusOnePlusOne(b[2]);

		return result;
	}

	static uint16_t* Vec4Com16bit(const glm::vec4& v)
	{
		glm::vec3 vec = glm::vec3(v.x / PosRange, v.y / PosRange, v.z / PosRange);

		return Vec3To16bit(vec);
	}

	static glm::vec4 Vec4Decom16bit(const uint16_t* b)
	{
		glm::vec3 vec = I6bitToVec3(b);

		glm::vec4 result;
		result.x = vec.x * PosRange;
		result.y = vec.y * PosRange;
		result.z = vec.z * PosRange;
		result.w = 0.0f;

		return result;
	}

	//glm::quat DecompressRotationKey(const AnimCurveKey& key)
	//{
	//	const uint16_t* keyData = key.KeyData;

	//	return QuatIhm16bit(keyData);
	//}

	//glm::vec4 DecompressPositionKey(const AnimCurveKey& key)
	//{
	//	const uint16_t* keyData = key.KeyData;

	//	return Vec4Decom16bit(keyData);
	//}
};


class AssimpGLMHelpers
{
public:

	static inline glm::mat4 ConvertMatrixToGLMFormat(const aiMatrix4x4& from)
	{
		glm::mat4 to;
		//the a,b,c,d in assimp is the row ; the 1,2,3,4 is the column
		to[0][0] = from.a1; to[1][0] = from.a2; to[2][0] = from.a3; to[3][0] = from.a4;
		to[0][1] = from.b1; to[1][1] = from.b2; to[2][1] = from.b3; to[3][1] = from.b4;
		to[0][2] = from.c1; to[1][2] = from.c2; to[2][2] = from.c3; to[3][2] = from.c4;
		to[0][3] = from.d1; to[1][3] = from.d2; to[2][3] = from.d3; to[3][3] = from.d4;
		return to;
	}

	static inline glm::vec3 GetGLMVec(const aiVector3D& vec)
	{
		return glm::vec3(vec.x, vec.y, vec.z);
	}

	static inline glm::quat GetGLMQuat(const aiQuaternion& pOrientation)
	{
		return glm::quat(pOrientation.w, pOrientation.x, pOrientation.y, pOrientation.z);
	}
};