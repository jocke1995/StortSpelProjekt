#include "stdafx.h"
#include "AnimatedModel.h"
#include "Animation.h"
#include "GPUMemory/ConstantBuffer.h"

#include "DescriptorHeap.h"

AnimatedModel::AnimatedModel(
	const std::wstring* path,
	SkeletonNode* rootNode,
	std::vector<Mesh*>* meshes,
	std::vector<Animation*>* animations,
	std::vector<Material*>* materials,
	unsigned int numBones)
	: Model(path, meshes, materials)
{
	m_Id = s_AnimatedModelIdCounter++;

	m_pSkeleton = rootNode;
	m_Animations = (*animations);
	m_UploadMatrices.reserve(numBones);

	for (unsigned int i = 0; i < numBones; i++)
	{
		m_UploadMatrices.push_back(DirectX::XMMatrixIdentity());
	}

	if (!m_Animations.empty())
	{
		m_pActiveAnimation = m_Animations[0];
	}
	else
	{
		m_pActiveAnimation = nullptr;
	}

	if (rootNode)
	{
		// Store the globalInverse transform.
		DirectX::XMMATRIX globalInverse = DirectX::XMLoadFloat4x4(&rootNode->defaultTransform);
		globalInverse = DirectX::XMMatrixInverse(nullptr, globalInverse);
		DirectX::XMStoreFloat4x4(&m_GlobalInverseTransform, globalInverse);
	}
}

AnimatedModel::~AnimatedModel()
{
	delete m_pSkeleton;
	delete m_pCB;
}

void AnimatedModel::InitConstantBuffer(ID3D12Device5* device5, DescriptorHeap* CBV_UAV_SRV_heap)
{
	
	std::string temp = to_string(m_Path);
	temp = temp.substr(temp.find_last_of("/\\") + 1);
	std::wstring resourceName = L"CB_Matrices_" + to_wstring(temp);
	m_pCB = new ConstantBuffer(device5, sizeof(ANIMATION_MATRICES_STRUCT), resourceName, CBV_UAV_SRV_heap);
}

const ConstantBuffer* AnimatedModel::GetConstantBuffer() const
{
	return m_pCB;
}

void AnimatedModel::Update(double dt)
{
	if (m_pActiveAnimation != nullptr)
	{
		float timeInTicks = dt * m_pActiveAnimation->ticksPerSecond;

		float animationTime = fmod(timeInTicks, m_pActiveAnimation->durationInTicks);
		m_pActiveAnimation->Update(animationTime);
		updateSkeleton(animationTime, m_pSkeleton, DirectX::XMMatrixIdentity());
	}
}

void AnimatedModel::updateSkeleton(float animationTime, SkeletonNode* node, DirectX::XMMATRIX parentTransform)
{
	m_pActiveAnimation->currentState[node->name].transform;
	DirectX::XMMATRIX transform;

	transform = DirectX::XMLoadFloat4x4(&node->defaultTransform);

	if (node->currentStateTransform)
	{
		DirectX::XMVECTOR position, rotationQ, scale, rotationOrigin;
		DirectX::XMLoadFloat3(&node->currentStateTransform->position);
		DirectX::XMLoadFloat4(&node->currentStateTransform->rotationQuaternion);
		DirectX::XMLoadFloat3(&node->currentStateTransform->scaling);
		rotationOrigin = { 0.0f,0.0f,0.0f };
		transform = DirectX::XMMatrixAffineTransformation(scale, rotationOrigin, rotationQ, position);
	}

	transform = parentTransform * transform;

	for (unsigned int i = 0; i < node->children.size(); i++)
	{
		updateSkeleton(animationTime, node->children[i], transform);
	}

	DirectX::XMMATRIX globalInverse = DirectX::XMLoadFloat4x4(&m_GlobalInverseTransform);
	DirectX::XMMATRIX inverseBindPose = DirectX::XMLoadFloat4x4(&node->inverseBindPose);
	transform = globalInverse * transform * inverseBindPose;

	m_UploadMatrices[node->boneID] = transform;
	DirectX::XMStoreFloat4x4(&node->modelSpaceTransform, transform);
}
