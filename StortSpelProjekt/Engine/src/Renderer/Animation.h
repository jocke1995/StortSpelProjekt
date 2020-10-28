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
	DirectX::XMFLOAT3 position = {};
	DirectX::XMFLOAT4 rotationQuaternion = {};
	DirectX::XMFLOAT3 scaling = {};
};

struct TranslationKey
{
	float time;
	DirectX::XMFLOAT3 position;
};
struct RotationKey
{
	float time;
	DirectX::XMFLOAT4 rotationQuaternion;
};
struct ScalingKey
{
	float time;
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
	int boneID;
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
	std::map<std::string, TransformKey> currentState;
	std::map<std::string, std::vector<TranslationKey>> translationKeyframes;
	std::map<std::string, std::vector<RotationKey>> rotationKeyframes;
	std::map<std::string, std::vector<ScalingKey>> scalingKeyframes;

	void Update(float animationTime)	// Interpolates the matrices and stores the finished animation as the current state
	{
		for (auto& bone : translationKeyframes)
		{
			assert(!bone.second.empty());
			// If there is only one key we can't interpolate
			if (bone.second.size() == 1)
			{
				currentState[bone.first].position = bone.second[0].position;
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
						Log::Print("TranslationIndex: %d\n", keyIndex);
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

				currentState[bone.first].position = InterpolateTranslation(&bone.second[keyIndex].position, &bone.second[nextKeyIndex].position, factor);
			}
		}

		for (auto& bone : rotationKeyframes)
		{
			assert(!bone.second.empty());
			// If there is only one key we can't interpolate
			if (bone.second.size() == 1)
			{
				currentState[bone.first].rotationQuaternion = bone.second[0].rotationQuaternion;
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
						Log::Print("RotationIndex: %d\n", keyIndex);
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

				currentState[bone.first].rotationQuaternion = InterpolateRotation(&bone.second[keyIndex].rotationQuaternion, &bone.second[nextKeyIndex].rotationQuaternion, factor);
			}
		}

		for (auto& bone : scalingKeyframes)
		{
			assert(!bone.second.empty());
			// If there is only one key we can't interpolate
			if (bone.second.size() == 1)
			{
				currentState[bone.first].scaling = bone.second[0].scaling;
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
						Log::Print("ScalingIndex: %d\n", keyIndex);
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

				currentState[bone.first].scaling = InterpolateScaling(&bone.second[keyIndex].scaling, &bone.second[nextKeyIndex].scaling, factor);
			}
		}
	}

	DirectX::XMFLOAT3 InterpolateTranslation(DirectX::XMFLOAT3* key0, DirectX::XMFLOAT3* key1, float t)
	{
		DirectX::XMFLOAT3 result;
		DirectX::XMVECTOR key0Vec;
		DirectX::XMVECTOR key1Vec;
		DirectX::XMVECTOR interpolatedVec;

		key0Vec = DirectX::XMLoadFloat3(key0);
		key1Vec = DirectX::XMLoadFloat3(key1);
		interpolatedVec = DirectX::XMVectorLerp(key0Vec, key1Vec, t);
		DirectX::XMStoreFloat3(&result, interpolatedVec);

		return result;
	}

	DirectX::XMFLOAT4 InterpolateRotation(DirectX::XMFLOAT4* key0, DirectX::XMFLOAT4* key1, float t)
	{
		DirectX::XMFLOAT4 result;
		DirectX::XMVECTOR key0Vec;
		DirectX::XMVECTOR key1Vec;
		DirectX::XMVECTOR interpolatedVec;

		key0Vec = DirectX::XMLoadFloat4(key0);
		key1Vec = DirectX::XMLoadFloat4(key1);
		interpolatedVec = DirectX::XMQuaternionSlerp(key0Vec, key1Vec, t);
		DirectX::XMQuaternionNormalize(interpolatedVec);
		DirectX::XMStoreFloat4(&result, interpolatedVec);

		return result;
	}

	DirectX::XMFLOAT3 InterpolateScaling(DirectX::XMFLOAT3* key0, DirectX::XMFLOAT3* key1, float t)
	{
		DirectX::XMFLOAT3 result;
		DirectX::XMVECTOR key0Vec;
		DirectX::XMVECTOR key1Vec;
		DirectX::XMVECTOR interpolatedVec;

		key0Vec = DirectX::XMLoadFloat3(key0);
		key1Vec = DirectX::XMLoadFloat3(key1);
		interpolatedVec = DirectX::XMVectorLerp(key0Vec, key1Vec, t);
		DirectX::XMStoreFloat3(&result, interpolatedVec);

		return result;
	}
};

#endif
