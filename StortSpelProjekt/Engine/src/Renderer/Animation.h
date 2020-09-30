#ifndef ANIMATION_H
#define ANIMATION_H

#define MAX_BONES_PER_VERTEX 10

struct VertexWeight
{
	unsigned int boneIDs[MAX_BONES_PER_VERTEX];
	float weights[MAX_BONES_PER_VERTEX];
};

struct SkeletonNode
{
	std::string name;
	std::vector<SkeletonNode*> children;
	DirectX::XMFLOAT4X4 defaultTransformation;
	DirectX::XMFLOAT4X4 inverseBindPoseMatrix;
	DirectX::XMFLOAT4X4 modelSpaceTransform;

	~SkeletonNode()
	{
		for (auto& child : children)
		{
			delete child;
		}
		children.clear();
	}
};

struct Float3Key
{
	float time;
	DirectX::XMFLOAT3 xyz;
};

struct Float4Key
{
	float time;
	DirectX::XMFLOAT4 xyzw;
};

struct NodeAnimation
{
	std::string name;
	std::vector<Float3Key> positions;
	std::vector<Float4Key> rotationQuaternions;
	std::vector<Float3Key> scalings;
};

struct Animation
{
	double durationInTicks;
	double ticksPerSecond;
	std::map<std::string, NodeAnimation> nodeAnimations;
};








#endif