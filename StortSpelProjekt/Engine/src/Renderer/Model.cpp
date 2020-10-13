#include "stdafx.h"
#include "Model.h"
#include "Mesh.h"
#include "Material.h"
#include "Texture/Texture2D.h"
#include "GPUMemory/ShaderResourceView.h"
#include "structs.h"
#include "Animation.h"

Model::Model(const std::wstring* path, SkeletonNode* rootNode, std::map<unsigned int, VertexWeight>* perVertexBoneData, std::vector<Mesh*>* meshes, std::vector<Animation*>* animations, std::vector<Material*>* materials)
{
	m_Path = *path;
	m_pSkeleton = rootNode;
	m_PerVertexBoneData = *perVertexBoneData;
	m_Size = (*meshes).size();

	m_Meshes = (*meshes);
	m_Animations = (*animations);
	m_Materials = (*materials);

	// Fill slotinfo with empty slotinfos
	m_SlotInfos.resize(m_Size);

	// TEMP
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

Model::~Model()
{
	delete m_pSkeleton;
}

void Model::Update(double dt)
{
	if (m_pActiveAnimation != nullptr)
	{
		float timeInTicks = dt * m_pActiveAnimation->ticksPerSecond;

		float animationTime = fmod(timeInTicks, m_pActiveAnimation->durationInTicks);
		m_pActiveAnimation->Update(animationTime);
		updateSkeleton(animationTime, m_pSkeleton, DirectX::XMMatrixIdentity());
	}
}

const std::wstring& Model::GetPath() const
{
	return m_Path;
}

unsigned int Model::GetSize() const
{
	return m_Size;
}

Mesh* Model::GetMeshAt(unsigned int index) const
{
	return m_Meshes[index];
}

Material* Model::GetMaterialAt(unsigned int index) const
{
	return m_Materials[index];;
}

const SlotInfo* Model::GetSlotInfoAt(unsigned int index) const
{
	return &m_SlotInfos[index];
}

void Model::updateSkeleton(float animationTime, SkeletonNode* node, DirectX::XMMATRIX parentTransform)
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

	DirectX::XMStoreFloat4x4(&node->modelSpaceTransform, transform);
}

double3 Model::GetModelDim() const
{
	double3 minVertex = { 100.0, 100.0, 100.0 }, maxVertex = { -100.0, -100.0, -100.0 };
	for (unsigned int i = 0; i < m_Size; i++)
	{
		std::vector<Vertex> modelVertices = *m_Meshes[i]->GetVertices();
		for (unsigned int i = 0; i < modelVertices.size(); i++)
		{
			minVertex.x = Min(minVertex.x, static_cast<double>(modelVertices[i].pos.x));
			minVertex.y = Min(minVertex.y, static_cast<double>(modelVertices[i].pos.y));
			minVertex.z = Min(minVertex.z, static_cast<double>(modelVertices[i].pos.z));

			maxVertex.x = Max(maxVertex.x, static_cast<double>(modelVertices[i].pos.x));
			maxVertex.y = Max(maxVertex.y, static_cast<double>(modelVertices[i].pos.y));
			maxVertex.z = Max(maxVertex.z, static_cast<double>(modelVertices[i].pos.z));
		}
	}
	return { maxVertex.x - minVertex.x, maxVertex.y - minVertex.y, maxVertex.z - minVertex.z };
}
