#include "stdafx.h"
#include "Model.h"
#include "Mesh.h"
#include "Texture.h"
#include "GPUMemory/ShaderResourceView.h"
#include "structs.h"
#include "Animation.h"

Model::Model(const std::wstring path, SkeletonNode* rootNode, std::map<unsigned int, VertexWeight>* perVertexBoneData, std::vector<Mesh*>* meshes, std::vector<Animation*>* animations, std::vector<std::map<TEXTURE_TYPE, Texture*>>* textures)
{
	m_Path = path;
	m_pSkeleton = rootNode;
	m_PerVertexBoneData = *perVertexBoneData;
	m_Size = (*meshes).size();

	m_Meshes = (*meshes);
	m_Animations = (*animations);
	m_Textures = (*textures);

	// Fill SlotInfo with mesh+material info
	for (unsigned int i = 0; i < (*meshes).size(); i++)
	{
		m_SlotInfos.push_back(
			{
			(*meshes)[i]->m_pSRV->GetDescriptorHeapIndex(),
			(*textures)[i][TEXTURE_TYPE::AMBIENT]->GetDescriptorHeapIndex(),
			(*textures)[i][TEXTURE_TYPE::DIFFUSE]->GetDescriptorHeapIndex(),
			(*textures)[i][TEXTURE_TYPE::SPECULAR]->GetDescriptorHeapIndex(),
			(*textures)[i][TEXTURE_TYPE::NORMAL]->GetDescriptorHeapIndex(),
			(*textures)[i][TEXTURE_TYPE::EMISSIVE]->GetDescriptorHeapIndex(),
			});
	}
}

Model::~Model()
{
	delete m_pSkeleton;
}

std::wstring Model::GetPath() const
{
	return m_Path;
}

unsigned int Model::GetSize() const
{
	return m_Size;
}

Mesh* Model::GetMeshAt(unsigned int index)
{
	return m_Meshes[index];
}

std::map<TEXTURE_TYPE, Texture*>* Model::GetTexturesAt(unsigned int index)
{
	return &m_Textures[index];
}

SlotInfo* Model::GetSlotInfoAt(unsigned int index)
{
	return &m_SlotInfos[index];
}

void Model::updateAnimations()
{
	if (m_pActiveAnimation != nullptr)
	{
		float animationTime = fmod(m_pActiveAnimation->durationInTicks, m_pActiveAnimation->ticksPerSecond);
		updateBones(animationTime, m_pSkeleton, DirectX::XMMatrixIdentity());
	}
}

void Model::updateBones(float animationTime, SkeletonNode* node, DirectX::XMMATRIX parentTransform)
{
	NodeAnimation* nodeAnimation = &m_pActiveAnimation->nodeAnimations[node->name];

	const DirectX::XMFLOAT4X4 defaultTransform = node->defaultTransformation;
	DirectX::XMMATRIX modelSpaceTransform = DirectX::XMLoadFloat4x4(&defaultTransform);

	if (nodeAnimation)
	{
		DirectX::XMMATRIX scaleMatrix = interpolateScaling(animationTime, nodeAnimation);

		DirectX::XMMATRIX rotationMatrix = interpolateRotation(animationTime, nodeAnimation);

		DirectX::XMMATRIX translationMatrix = interpolateTranslation(animationTime, nodeAnimation);

		modelSpaceTransform = translationMatrix * rotationMatrix * scaleMatrix;
	}

	modelSpaceTransform = parentTransform * modelSpaceTransform;

	// Find the correct bone (skeleton node)

	// Store the new transform in the bone

	// Update the child bones. The children will be dependent on their parent transform.
	for (unsigned int i = 0; i < node->children.size(); i++)
	{
		updateBones(animationTime, node->children[i], modelSpaceTransform);
	}

	// Multiply with inverse bind
}

DirectX::XMMATRIX Model::interpolateScaling(float animationTime, NodeAnimation* nodeAnimation)
{
	assert(!nodeAnimation->scalings.empty());

	// If there is only one key we can't interpolate
	if (nodeAnimation->scalings.size() == 1)
	{
		const DirectX::XMFLOAT3 key = nodeAnimation->scalings[0].xyz;
		DirectX::XMVECTOR keyVec = DirectX::XMLoadFloat3(&key);
		return DirectX::XMMatrixScalingFromVector(keyVec);
	}

	// Find the current key index
	unsigned int scalingIndex;
	for (unsigned int i = 0; i < nodeAnimation->scalings.size(); i++)
	{
		if (animationTime < nodeAnimation->scalings[i].time)
		{
			scalingIndex = i;
		}
	}
	unsigned int nextScalingIndex = scalingIndex + 1;
	assert(nextScalingIndex < nodeAnimation->scalings.size());

	// Calculate interpolation factor
	float dt = nodeAnimation->scalings[nextScalingIndex].time - nodeAnimation->scalings[scalingIndex].time;
	float factor = (animationTime - nodeAnimation->scalings[scalingIndex].time) / dt;
	assert(factor >= 0.0f && factor <= 1.0f);

	// Interpolate the keys and return the matrix
	const DirectX::XMFLOAT3 key1Float3 = nodeAnimation->scalings[scalingIndex].xyz;
	DirectX::XMVECTOR key1 = DirectX::XMLoadFloat3(&key1Float3);
	const DirectX::XMFLOAT3 key2Float3 = nodeAnimation->scalings[nextScalingIndex].xyz;
	DirectX::XMVECTOR key2 = DirectX::XMLoadFloat3(&key2Float3);

	DirectX::XMVECTOR interpolatedKey = DirectX::XMVectorLerp(key1, key2, factor);

	// Maybe normalize
	return DirectX::XMMatrixScalingFromVector(interpolatedKey);
}

DirectX::XMMATRIX Model::interpolateRotation(float animationTime, NodeAnimation* nodeAnimation)
{
	assert(!nodeAnimation->rotationQuaternions.empty());

	// If there is only one key we can't interpolate
	if (nodeAnimation->rotationQuaternions.size() == 1)
	{
		const DirectX::XMFLOAT4 key = nodeAnimation->rotationQuaternions[0].xyzw;
		DirectX::XMVECTOR keyVec = DirectX::XMLoadFloat4(&key);
		return DirectX::XMMatrixRotationQuaternion(keyVec);
	}

	// Find the current key index
	unsigned int rotationIndex;
	for (unsigned int i = 0; i < nodeAnimation->rotationQuaternions.size(); i++)
	{
		if (animationTime < nodeAnimation->rotationQuaternions[i].time)
		{
			rotationIndex = i;
		}
	}
	unsigned int nextRotationIndex = rotationIndex + 1;
	assert(nextRotationIndex < nodeAnimation->rotationQuaternions.size());

	// Calculate interpolation factor
	float dt = nodeAnimation->rotationQuaternions[nextRotationIndex].time - nodeAnimation->rotationQuaternions[rotationIndex].time;
	float factor = (animationTime - nodeAnimation->rotationQuaternions[rotationIndex].time) / dt;
	assert(factor >= 0.0f && factor <= 1.0f);

	// Interpolate the keys and return the matrix
	const DirectX::XMFLOAT4 key1Float4 = nodeAnimation->rotationQuaternions[rotationIndex].xyzw;
	DirectX::XMVECTOR key1 = DirectX::XMLoadFloat4(&key1Float4);
	const DirectX::XMFLOAT4 key2Float4 = nodeAnimation->rotationQuaternions[nextRotationIndex].xyzw;
	DirectX::XMVECTOR key2 = DirectX::XMLoadFloat4(&key2Float4);

	DirectX::XMVECTOR interpolatedKey = DirectX::XMQuaternionSlerp(key1, key2, factor);
	
	// Maybe normalize
	return DirectX::XMMatrixRotationQuaternion(interpolatedKey);
}

DirectX::XMMATRIX Model::interpolateTranslation(float animationTime, NodeAnimation* nodeAnimation)
{
	assert(!nodeAnimation->positions.empty());

	// If there is only one key we can't interpolate
	if (nodeAnimation->positions.size() == 1)
	{
		const DirectX::XMFLOAT3 key = nodeAnimation->positions[0].xyz;
		DirectX::XMVECTOR keyVec = DirectX::XMLoadFloat3(&key);
		return DirectX::XMMatrixScalingFromVector(keyVec);
	}

	// Find the current key index
	unsigned int positionIndex;
	for (unsigned int i = 0; i < nodeAnimation->positions.size(); i++)
	{
		if (animationTime < nodeAnimation->positions[i].time)
		{
			positionIndex = i;
		}
	}
	unsigned int nextPositionIndex = positionIndex + 1;
	assert(nextPositionIndex < nodeAnimation->positions.size());

	// Calculate interpolation factor
	float dt = nodeAnimation->positions[nextPositionIndex].time - nodeAnimation->positions[positionIndex].time;
	float factor = (animationTime - nodeAnimation->positions[positionIndex].time) / dt;
	assert(factor >= 0.0f && factor <= 1.0f);

	// Interpolate the keys and return the matrix
	const DirectX::XMFLOAT3 key1Float3 = nodeAnimation->positions[positionIndex].xyz;
	DirectX::XMVECTOR key1 = DirectX::XMLoadFloat3(&key1Float3);
	const DirectX::XMFLOAT3 key2Float3 = nodeAnimation->positions[nextPositionIndex].xyz;
	DirectX::XMVECTOR key2 = DirectX::XMLoadFloat3(&key2Float3);

	DirectX::XMVECTOR interpolatedKey = DirectX::XMVectorLerp(key1, key2, factor);

	// Maybe normalize
	return DirectX::XMMatrixScalingFromVector(interpolatedKey);
}
