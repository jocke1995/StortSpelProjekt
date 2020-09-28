#ifndef ANIMATION_H
#define ANIMATION_H

struct VertexWeight
{
	unsigned int vertexID;
	float weight;
};

struct BoneNode
{
	std::string name;
	BoneNode* parent;
	std::vector<BoneNode*> children;
	std::vector<VertexWeight> weights;
	DirectX::XMFLOAT4X4 offsetMatrix;
	DirectX::XMFLOAT4X4 defaultTransformation;
	DirectX::XMFLOAT4X4 finalTransformation;
};

struct NodeAnimation
{
	std::string name;
	std::vector<DirectX::XMFLOAT3> positions;	// Maybe should not be float3
	std::vector<DirectX::XMFLOAT4> rotationQuaternions;
	std::vector<DirectX::XMFLOAT3> scalings;	// Maybe should not be float3
};

struct Animation
{
	double durationInTicks;
	double ticksPerSecond;
	std::map<std::string, NodeAnimation> nodeAnimations;
};








#endif