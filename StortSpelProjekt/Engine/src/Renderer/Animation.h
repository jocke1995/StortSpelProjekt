#ifndef ANIMATION_H
#define ANIMATION_H

#define MAX_BONES_PER_VERTEX 10
#define ANIMATION_TRANSITION_TIME 0.2f

DirectX::XMFLOAT3 InterpolateTranslation(DirectX::XMFLOAT3* key0, DirectX::XMFLOAT3* key1, float t);
DirectX::XMFLOAT4 InterpolateRotation(DirectX::XMFLOAT4* key0, DirectX::XMFLOAT4* key1, float t);
DirectX::XMFLOAT3 InterpolateScaling(DirectX::XMFLOAT3* key0, DirectX::XMFLOAT3* key1, float t);

struct BoneInfo
{
	unsigned int boneID;
	DirectX::XMFLOAT4X4 boneOffset;
};

struct TransformKey
{
	DirectX::XMFLOAT3 position = {};
	DirectX::XMFLOAT3* pPosition = &position;
	DirectX::XMFLOAT4 rotation = {};
	DirectX::XMFLOAT4* pRotation = &rotation;
	DirectX::XMFLOAT3 scaling = { 1.0f,1.0f,1.0f };
	DirectX::XMFLOAT3* pScale = &scaling;
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

struct SkeletonNode
{
	std::string name;
	int boneID;
	std::vector<BoneInfo> bones;
	std::vector<SkeletonNode*> children;

	DirectX::XMFLOAT4X4 defaultTransform;
	DirectX::XMFLOAT4X4 inverseBindPose;	// Bone offset (might be wrong)
	TransformKey* currentStateTransform;

	SkeletonNode()
	{
		boneID = -1;
		DirectX::XMStoreFloat4x4(&defaultTransform, DirectX::XMMatrixIdentity());
		DirectX::XMStoreFloat4x4(&inverseBindPose, DirectX::XMMatrixIdentity());
		currentStateTransform = nullptr;
	}
	
	~SkeletonNode()
	{
		for (auto& child : children)
		{
			delete child;
		}
		children.clear();
	}

	SkeletonNode(const SkeletonNode& other)
	{
		name = other.name;
		boneID = other.boneID;
		bones = other.bones;
		defaultTransform = other.defaultTransform;
		inverseBindPose = other.inverseBindPose;
		currentStateTransform = nullptr;
	}

	// Clones the skeleton. Uses dynamic memory allocation so make sure to delete the clone.
	SkeletonNode* Clone()
	{
		SkeletonNode* nodeCopy = new SkeletonNode(*this);
		for (auto& child : children)
		{
			nodeCopy->children.push_back(child->Clone());
		}
		return nodeCopy;
	}
};

struct AnimationInfo
{
	bool loop = true;
	bool finished = false;
	double animationTime = 0.0f;

	void Reset()
	{
		loop = true;
		finished = false;
		animationTime = 0.0f;
	}
};

struct Animation
{
	std::string name;
	double durationInTicks;
	double ticksPerSecond;
	std::map<std::string, std::vector<TranslationKey>> translationKeyframes;
	std::map<std::string, std::vector<RotationKey>> rotationKeyframes;
	std::map<std::string, std::vector<ScalingKey>> scalingKeyframes;

	bool isFinished(double animationTime)
	{
		if (animationTime * ticksPerSecond > durationInTicks - (ANIMATION_TRANSITION_TIME * ticksPerSecond))
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	void Update(double animationTime, std::map<std::string, TransformKey>& state)	// Interpolates the matrices and stores the finished animation as the current state
	{
		double animationTimeInTicks = fmod(animationTime * ticksPerSecond, durationInTicks);

		for (auto& bone : translationKeyframes)
		{
			assert(!bone.second.empty());
			// If there is only one key we can't interpolate
			if (bone.second.size() > 1)
			{
				// Find the current key index
				unsigned int keyIndex = 0;
				for (unsigned int i = 0; i < bone.second.size() - 1; i++)
				{
					if (animationTimeInTicks < bone.second[i + 1].time)
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
				float factor = (animationTimeInTicks - bone.second[keyIndex].time) / dt;
				assert(factor >= 0.0f && factor <= 1.0f);

				state[bone.first].position = InterpolateTranslation(&bone.second[keyIndex].position, &bone.second[nextKeyIndex].position, factor);
			}
		}

		for (auto& bone : rotationKeyframes)
		{
			assert(!bone.second.empty());
			// If there is only one key we can't interpolate
			if (bone.second.size() > 1)
			{
				// Find the current key index
				unsigned int keyIndex = 0;
				for (unsigned int i = 0; i < bone.second.size() - 1; i++)
				{
					if (animationTimeInTicks < bone.second[i + 1].time)
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
				float factor = (animationTimeInTicks - bone.second[keyIndex].time) / dt;
				assert(factor >= 0.0f && factor <= 1.0f);

				state[bone.first].rotation = InterpolateRotation(&bone.second[keyIndex].rotationQuaternion, &bone.second[nextKeyIndex].rotationQuaternion, factor);
			}
		}

		for (auto& bone : scalingKeyframes)
		{
			assert(!bone.second.empty());
			// If there is only one key we can't interpolate
			if (bone.second.size() > 1)
			{
				// Find the current key index
				unsigned int keyIndex = 0;
				for (unsigned int i = 0; i < bone.second.size() - 1; i++)
				{
					if (animationTimeInTicks < bone.second[i + 1].time)
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
				float factor = (animationTimeInTicks - bone.second[keyIndex].time) / dt;
				assert(factor >= 0.0f && factor <= 1.0f);

				state[bone.first].scaling = InterpolateScaling(&bone.second[keyIndex].scaling, &bone.second[nextKeyIndex].scaling, factor);
			}
		}
	}
};

inline DirectX::XMFLOAT3 InterpolateTranslation(DirectX::XMFLOAT3* key0, DirectX::XMFLOAT3* key1, float t)
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

inline DirectX::XMFLOAT4 InterpolateRotation(DirectX::XMFLOAT4* key0, DirectX::XMFLOAT4* key1, float t)
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

inline DirectX::XMFLOAT3 InterpolateScaling(DirectX::XMFLOAT3* key0, DirectX::XMFLOAT3* key1, float t)
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

#endif
