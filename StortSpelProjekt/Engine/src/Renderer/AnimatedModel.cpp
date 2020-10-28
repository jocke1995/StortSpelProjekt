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

const std::vector<DirectX::XMMATRIX>* AnimatedModel::GetUploadMatrices() const
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
	DirectX::XMMATRIX transform;
	if (node->boneID != -1)
	{
		transform = DirectX::XMLoadFloat4x4(&node->defaultTransform);

		if (node->currentStateTransform)
		{
			DirectX::XMVECTOR position, rotationQ, scale, rotationOrigin;
			position = DirectX::XMLoadFloat3(&node->currentStateTransform->position);
			rotationQ = DirectX::XMLoadFloat4(&node->currentStateTransform->rotationQuaternion);
			scale = DirectX::XMLoadFloat3(&node->currentStateTransform->scaling);
			DirectX::XMMATRIX trans = DirectX::XMMatrixTranslationFromVector(position);
			DirectX::XMMATRIX rot = DirectX::XMMatrixRotationQuaternion(rotationQ);
			DirectX::XMMATRIX scal = DirectX::XMMatrixScalingFromVector(scale);
			transform = scal * rot * trans;

			//rotationOrigin = { 0.0f,0.0f,0.0f };
			//transform = DirectX::XMMatrixAffineTransformation(scale, rotationOrigin, rotationQ, position);
		}
		transform = transform * parentTransform;

		DirectX::XMMATRIX globalInverse = DirectX::XMLoadFloat4x4(&m_GlobalInverseTransform);
		DirectX::XMMATRIX inverseBindPose = DirectX::XMLoadFloat4x4(&node->inverseBindPose);

		m_UploadMatrices[node->boneID] = transform * inverseBindPose * globalInverse;// *transform; //globalInverse * 
		m_UploadMatrices[node->boneID] = DirectX::XMMatrixTranspose(m_UploadMatrices[node->boneID]);

		DirectX::XMStoreFloat4x4(&node->modelSpaceTransform, m_UploadMatrices[node->boneID]);
	}
	else
	{
		transform = parentTransform;
	}

	for (unsigned int i = 0; i < node->children.size(); i++)
	{
		updateSkeleton(animationTime, node->children[i], transform);
	}
}
