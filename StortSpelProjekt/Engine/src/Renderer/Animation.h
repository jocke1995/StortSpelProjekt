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
	DirectX::XMFLOAT4X4 finalTransform;
	DirectX::XMFLOAT4X4 offsetMatrix;
};

struct NodeTemp
{
	std::string name;
	std::vector<NodeTemp*> children;
	DirectX::XMFLOAT4X4 defaultTransformation;
	DirectX::XMFLOAT4X4 finalTransformation;
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