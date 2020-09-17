#ifndef ANIMATION_H
#define ANIMATION_H

struct VertexWeight
{
	unsigned int vertexID;
	float weight;
};

struct Bone
{
	std::string name;
	std::vector<VertexWeight> weights;
	DirectX::XMFLOAT4X4 offsetMatrix;
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
	double duration;
	double ticksPerSecond;
	std::vector<NodeAnimation> nodeAnimations;
};








#endif