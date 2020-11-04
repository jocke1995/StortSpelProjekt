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

	DirectX::XMFLOAT4X4 matIdentity;
	DirectX::XMStoreFloat4x4(&matIdentity, DirectX::XMMatrixIdentity());

	for (unsigned int i = 0; i < numBones; i++)
	{
		m_UploadMatrices.push_back(matIdentity);
	}

	m_pActiveAnimation = m_Animations[0];

	// Store the globalInverse transform.
	DirectX::XMMATRIX globalInverse = DirectX::XMLoadFloat4x4(&rootNode->defaultTransform);
	globalInverse = DirectX::XMMatrixInverse(nullptr, globalInverse);
	DirectX::XMStoreFloat4x4(&m_GlobalInverseTransform, globalInverse);
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

const std::vector<DirectX::XMFLOAT4X4>* AnimatedModel::GetUploadMatrices() const
{
	return &m_UploadMatrices;
}

void AnimatedModel::Update(double dt)
{
	if (m_pActiveAnimation != nullptr)
	{
		static double time;
		time += dt;
		float timeInTicks = time * m_pActiveAnimation->ticksPerSecond;

		float animationTime = fmod(timeInTicks, m_pActiveAnimation->durationInTicks);
		m_pActiveAnimation->Update(animationTime);
		updateSkeleton(animationTime, m_pSkeleton, DirectX::XMMatrixIdentity());
	}
}

void AnimatedModel::updateSkeleton(float animationTime, SkeletonNode* node, DirectX::XMMATRIX parentTransform)
{
	DirectX::XMMATRIX localTransform;

	if (node->currentStateTransform) // node animated? could be optimized to only transform the parts (rot, scale or trans) that were actually animated --> example $AssimpFbx$_Rotation only afftect rotation part
	{
		DirectX::XMVECTOR position, rotationQ, scale, rotationOrigin;
		position = DirectX::XMLoadFloat3(&node->currentStateTransform->position);
		rotationQ = DirectX::XMLoadFloat4(&node->currentStateTransform->rotationQuaternion);
		scale = DirectX::XMLoadFloat3(&node->currentStateTransform->scaling);
		DirectX::XMMATRIX translationMatrix = DirectX::XMMatrixTranslationFromVector(position);
		DirectX::XMMATRIX rotationMatrix = DirectX::XMMatrixRotationQuaternion(rotationQ);
		DirectX::XMMATRIX scaleMatrix = DirectX::XMMatrixScalingFromVector(scale);
		localTransform = scaleMatrix * rotationMatrix * translationMatrix;
	}
	else // use src transformation otherwise
	{
		localTransform = DirectX::XMLoadFloat4x4(&node->defaultTransform);
	}

	DirectX::XMMATRIX finalTransform = localTransform * parentTransform;

	if (node->boneID != -1)
	{
		DirectX::XMMATRIX globalInverse = DirectX::XMLoadFloat4x4(&m_GlobalInverseTransform);
		DirectX::XMMATRIX inverseBindPose = DirectX::XMLoadFloat4x4(&node->inverseBindPose);

		DirectX::XMStoreFloat4x4(&m_UploadMatrices[node->boneID], DirectX::XMMatrixTranspose(inverseBindPose * finalTransform));// *transform; //globalInverse * 
	}

	for (unsigned int i = 0; i < node->children.size(); i++)
	{
		updateSkeleton(animationTime, node->children[i], finalTransform);
	}
}
