#ifndef ANIMATION_H
#define ANIMATION_H

#define MAX_BONES_PER_VERTEX 10

struct BoneInfo
{
	unsigned int boneID;
	DirectX::XMFLOAT4X4 boneOffset;
};

struct TransformKey
{
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT4 rotationQuaternion;
	DirectX::XMFLOAT3 scaling;
};

struct Keyframe
{
	float time;
	TransformKey transform;
};

struct SkeletonNode
{
	std::string name;
	unsigned int boneID;
	std::vector<BoneInfo> bones;
	std::vector<SkeletonNode*> children;

	DirectX::XMFLOAT4X4 defaultTransform;
	DirectX::XMFLOAT4X4 inverseBindPose;	// Bone offset
	DirectX::XMFLOAT4X4 modelSpaceTransform;
	TransformKey* currentStateTransform;

	~SkeletonNode()
	{
		for (auto& child : children)
		{
			delete child;
		}
		children.clear();
	}
};

struct Animation
{
	double durationInTicks;
	double ticksPerSecond;
	std::map<std::string, Keyframe> currentState;
	std::map<std::string, std::vector<Keyframe>> nodeAnimationKeyframes;

	void Update(float animationTime)	// Interpolates the matrices and stores the finished animation as the current state
	{
		for (auto& bone : nodeAnimationKeyframes)
		{
			assert(!bone.second.empty());
			// If there is only one key we can't interpolate
			if (bone.second.size() == 1)
			{
				currentState[bone.first] = bone.second[0];
			}
			else
			{
				// Find the current key index
				unsigned int keyIndex = 0;
				for (unsigned int i = 0; i < bone.second.size() - 1; i++)
				{
					if (animationTime < bone.second[i + 1].time)
					{
						keyIndex = i;
						break;
					}
				}
				unsigned int nextKeyIndex = keyIndex + 1;
				assert(nextKeyIndex < bone.second.size());
				assert(keyIndex >= 0);

				// Calculate interpolation factor
				float dt = bone.second[nextKeyIndex].time - bone.second[keyIndex].time;
				float factor = (animationTime - bone.second[keyIndex].time) / dt;
				assert(factor >= 0.0f && factor <= 1.0f);

				// Maybe normalize?
				currentState[bone.first] = InterpolateKeyframe(&bone.second[keyIndex], &bone.second[nextKeyIndex], factor);
			}
		}
	}

	Keyframe InterpolateKeyframe(Keyframe* key0, Keyframe* key1, float t)
	{
		Keyframe result;
		DirectX::XMVECTOR key0Vec;
		DirectX::XMVECTOR key1Vec;
		DirectX::XMVECTOR interpolatedVec;

		key0Vec = DirectX::XMLoadFloat3(&key0->transform.position);
		key1Vec = DirectX::XMLoadFloat3(&key1->transform.position);
		interpolatedVec = DirectX::XMVectorLerp(key0Vec, key1Vec, t);
		DirectX::XMStoreFloat3(&result.transform.position, interpolatedVec);

		key0Vec = DirectX::XMLoadFloat4(&key0->transform.rotationQuaternion);
		key1Vec = DirectX::XMLoadFloat4(&key1->transform.rotationQuaternion);
		interpolatedVec = DirectX::XMQuaternionSlerp(key0Vec, key1Vec, t);
		DirectX::XMStoreFloat4(&result.transform.rotationQuaternion, interpolatedVec);

		key0Vec = DirectX::XMLoadFloat3(&key0->transform.scaling);
		key1Vec = DirectX::XMLoadFloat3(&key1->transform.scaling);
		interpolatedVec = DirectX::XMVectorLerp(key0Vec, key1Vec, t);
		DirectX::XMStoreFloat3(&result.transform.scaling, interpolatedVec);

		return result;
	}
};

#endif
