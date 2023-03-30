#pragma once
#include <cstdint>
#include <string>
#include <vector>
#include "glm/glm.hpp"
#include "glm/gtx/compatibility.hpp"
#include "glm/gtx/rotate_vector.hpp"

static const uint32_t InitialFNV = 2166136261U;
static const uint32_t FNVMultiple = 16777619U;

struct Rig
{
	std::vector<HashedName> BoneNameHashes;
	std::vector<int16_t> Parents;
	std::vector<Xform> RefPoses;
	uint32_t NumBones;
};

struct HashedName
{
	uint32_t Value;

	HashedName(const char* name)
	{
		Value = InitialFNV;
		const uint64_t len = strlen(name);

		for (uint64_t i = 0; i < len; ++i)
		{
			Value = (Value ^ name[i]) * FNVMultiple;
		}
	}
};

struct Xform
{
	glm::vec4 Position;
	glm::quat Rotation;
};

struct Pose
{
	Xform* Xforms;
	float* Tracks;
	uint32_t NumXforms;
	uint32_t NumTracks;
};

struct AnimCurveKey
{
	uint16_t KeyTime;
	uint16_t KeyData[3];
};

struct AnimCurve
{
	uint16_t Index;
	uint16_t NumKeys;
	uint16_t NumPosKeys;
	AnimCurveKey* Keys;
	AnimCurveKey* PosKeys;
};

struct AnimClip
{
	HashedName Name;
	float Duration;
	uint32_t NumCurves;
	uint32_t NumDataKeys;
	AnimCurve* Curves;
	AnimCurveKey* DataKeys;
};

float DecompressKeyTime(const uint16_t time)
{
	return float(time) / 24.0f;
}

glm::vec4 fm_quat_rotate(const glm::quat& q, const glm::vec4& v)
{
	glm::vec3 uv, uuv;
	glm::vec3 qvec(q.x, q.y, q.z);
	uv = glm::cross(qvec, glm::vec3(v));
	uuv = glm::cross(qvec, uv);
	uv *= (2.0f * q.w);
	uuv *= 2.0f;
	return glm::vec4(glm::vec3(v) + uv + uuv, v.w);
}

Xform fm_xform_mul(const Xform& a, const Xform& b)
{
	Xform result;
	// Oblicz pozycjê wynikow¹
	result.Position = a.Position + fm_quat_rotate(b.Rotation, a.Position);
	
	// Oblicz obrót wynikowy
	result.Rotation = b.Rotation * a.Rotation;
	return result;
}


void PoseLocalToModel(Pose* modelPose, const Pose* localPose, const int16_t* parentIndices)
{
	const Xform* localXforms = localPose->Xforms;
	Xform* modelXforms = modelPose->Xforms;

	uint32_t numBones = std::min(modelPose->NumXforms, localPose->NumXforms);

	for (uint16_t i = 0; i < numBones; ++i)
	{
		const int16_t idxParent = parentIndices[i];
		if (idxParent >= 0)
		{
			// model-parent * local = model
			modelXforms[i] = fm_xform_mul(modelXforms[idxParent], localXforms[i]);
		}
		else
		{
			modelXforms[i] = localXforms[i];
		}
	}

	uint32_t numTracks = std::min(modelPose->NumTracks, localPose->NumTracks);
	if (numTracks > 0U)
	{
		memcpy(modelPose->Tracks, localPose->Tracks, sizeof(float) * numTracks);
	}
}

void AnimClipSample(const AnimClip* clip, float time, Pose* pose)
{
    // iterate each curve (bone) 
    const uint32_t numCurves = clip->NumCurves;

    for (uint32_t i = 0; i < numCurves; ++i)
    {
        const AnimCurve* curve = &clip->Curves[i];

        const uint16_t idxXform = curve->Index;

        // rotation
        {
            const uint16_t numKeys = curve->NumKeys;
            uint16_t idx = 0;

            // find upper index by time (this could be a binary search)
            while (idx < (numKeys - 1) && DecompressKeyTime(curve->Keys[idx].KeyTime) < time)
            {
                ++idx;
            }

            const uint16_t upperIdx = idx;
            const uint16_t lowerIdx = idx == 0 ? idx : idx - 1;

            glm::quat rot;

            if (lowerIdx == upperIdx)  // constant key 
            {
				rot = DecompressRotationKey(curve->Keys[idx]);
            }
            else  // at least two keys - interpolate 
            {
                glm::quat rot1 = DecompressRotationKey(curve->Keys[lowerIdx]);

				glm::quat rot2 = DecompressRotationKey(curve->Keys[upperIdx]);

                const float time1 = DecompressKeyTime(curve->Keys[lowerIdx].KeyTime);
                const float time2 = DecompressKeyTime(curve->Keys[upperIdx].KeyTime);

                float alpha = (time - time1) / (time2 - time1);
				rot = glm::lerp(rot1, rot2, alpha);

                // because we do LERP on quaternion (faster on CPU), we have to normalise it 
				rot = glm::normalize(rot);
            }

            pose->Xforms[idxXform].Rotation = rot;
        }

        // position
        {
            const uint16_t numKeys = curve->NumPosKeys;
            uint16_t idx = 0;

            // find upper index by time (this could be a binary search)
            while (idx < (numKeys - 1) && DecompressKeyTime(curve->PosKeys[idx].KeyTime) < time)
            {
                ++idx;
            }

            const uint16_t upperIdx = idx;
            const uint16_t lowerIdx = idx == 0 ? idx : idx - 1;

            glm::vec4 pos;

            if (lowerIdx == upperIdx)  // constant key 
            {
				pos = DecompressPositionKey(curve->PosKeys[idx]);
            }
            else  // at least two keys - interpolate 
            {
                glm::vec4 pos1 = DecompressPositionKey(curve->PosKeys[lowerIdx]);

                glm::vec4 pos2 = DecompressPositionKey(curve->PosKeys[upperIdx]);

                const float time1 = DecompressKeyTime(curve->PosKeys[lowerIdx].KeyTime);
                const float time2 = DecompressKeyTime(curve->PosKeys[upperIdx].KeyTime);

                float alpha = (time - time1) / (time2 - time1);
				pos = glm::lerp(pos2, pos1, alpha);
				
            }

            pose->Xforms[idxXform].Position = pos;
        }
    }
}