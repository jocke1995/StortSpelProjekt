#include "stdafx.h"
#include "BoundingBoxComponent.h"
#include "../Renderer/Mesh.h"

#include "../Entity.h"

// Creating the BB out of the model
#include "MeshComponent.h"

// Using the same m_pTransform as the models m_pTransform
#include "../Renderer/Transform.h"
#include "TransformComponent.h"
#include "../Renderer/BoundingBoxPool.h"

component::BoundingBoxComponent::BoundingBoxComponent(Entity* parent, unsigned int flagOBB)
	: Component(parent)
{
	m_FlagOBB = flagOBB;
}

component::BoundingBoxComponent::~BoundingBoxComponent()
{
}

void component::BoundingBoxComponent::Init()
{
	createOrientedBoundingBox();
}

void component::BoundingBoxComponent::Update(double dt)
{
	// update pos
	m_OrientedBoundingBox.Center.x = m_pTransform->GetPositionFloat3().x;
	m_OrientedBoundingBox.Center.y = m_pTransform->GetPositionFloat3().y;
	m_OrientedBoundingBox.Center.z = m_pTransform->GetPositionFloat3().z;

	//update rot
	DirectX::XMMATRIX wm = m_pTransform->GetRotMatrix();
	DirectX::XMVECTOR qv = DirectX::XMQuaternionRotationMatrix((wm));
	DirectX::XMStoreFloat4(&m_OrientedBoundingBox.Orientation, qv);
}

void component::BoundingBoxComponent::SetMesh(Mesh* mesh)
{
	m_pMesh = mesh;
}

DirectX::BoundingOrientedBox component::BoundingBoxComponent::GetOBB() const
{
	return m_OrientedBoundingBox;
}

Transform* component::BoundingBoxComponent::GetTransform() const
{
	return m_pTransform;
}

const Mesh* component::BoundingBoxComponent::GetMesh() const
{
	return m_pMesh;
}

const BoundingBoxData* component::BoundingBoxComponent::GetBoundingBoxData() const
{
	return m_pBbd;
}

const std::string component::BoundingBoxComponent::GetPathOfModel() const
{
	return m_pPathOfModel;
}

unsigned int component::BoundingBoxComponent::GetFlagOBB() const
{
	return m_FlagOBB;
}

// Writes from BoundingBoxComponent to MeshComponent, which uses this in m_pRenderer
bool& component::BoundingBoxComponent::IsPickedThisFrame()
{
	return m_pParent->GetComponent<MeshComponent>()->m_IsPickedThisFrame;
}

bool component::BoundingBoxComponent::createOrientedBoundingBox()
{
	if (m_pParent->HasComponent<MeshComponent>() == true && m_pParent->HasComponent<TransformComponent>() == true)
	{
		// Use the same m_pTransform as the model
		m_pTransform = m_pParent->GetComponent<TransformComponent>()->GetTransform();
		MeshComponent* mc = m_pParent->GetComponent<MeshComponent>();
		m_pPathOfModel = mc->GetMesh(0)->GetPath();

		BoundingBoxPool* bbp = BoundingBoxPool::Get();
		if (bbp->BoundingBoxDataExists(m_pPathOfModel) == true)
		{
			m_pBbd = bbp->GetBoundingBoxData(m_pPathOfModel);
			return true;
		}

		// Create new bounding box
		float3 minVertex = { MAXNUMBER, MAXNUMBER, MAXNUMBER };
		float3 maxVertex = { -MAXNUMBER, -MAXNUMBER, -MAXNUMBER };

		std::vector<Vertex> boundingBoxVerticesLocal;
		std::vector<unsigned int> boundingBoxIndicesLocal;

		for (unsigned int i = 0; i < mc->GetNrOfMeshes(); i++)
		{
			std::vector<Vertex> modelVertices = *mc->GetMesh(i)->GetVertices();

			for (unsigned int j = 0; j < modelVertices.size(); j++)
			{
				minVertex.x = Min(minVertex.x, modelVertices[j].pos.x);
				minVertex.y = Min(minVertex.y, modelVertices[j].pos.y);
				minVertex.z = Min(minVertex.z, modelVertices[j].pos.z);

				maxVertex.x = Max(maxVertex.x, modelVertices[j].pos.x);
				maxVertex.y = Max(maxVertex.y, modelVertices[j].pos.y);
				maxVertex.z = Max(maxVertex.z, modelVertices[j].pos.z);
			}
		}
		
		// Extents are from middle of box to the edge of the box in all axisis
		float3 absHalfLenghtOfRect = {	(abs(minVertex.x) + abs(maxVertex.x)) / 2 * m_pTransform->GetScale().x,
										(abs(minVertex.y) + abs(maxVertex.y)) / 2 * m_pTransform->GetScale().y,
										(abs(minVertex.z) + abs(maxVertex.z)) / 2 * m_pTransform->GetScale().z };
		m_OrientedBoundingBox.Extents.x = absHalfLenghtOfRect.x;
		m_OrientedBoundingBox.Extents.y = absHalfLenghtOfRect.y;
		m_OrientedBoundingBox.Extents.z = absHalfLenghtOfRect.z;

		// Set the position of the OBB
		m_OrientedBoundingBox.Center.x = m_pTransform->GetPositionFloat3().x;
		m_OrientedBoundingBox.Center.y = m_pTransform->GetPositionFloat3().y;
		m_OrientedBoundingBox.Center.z = m_pTransform->GetPositionFloat3().z;

		// get roataion from the rotationMatrix and convert it 
		// to the required quaternion(XMFLOAT4) and store it
		DirectX::XMMATRIX wm = m_pTransform->GetRotMatrix();
		DirectX::XMVECTOR qv = DirectX::XMQuaternionRotationMatrix(wm);
		DirectX::XMStoreFloat4(&m_OrientedBoundingBox.Orientation, qv);

		// Create bounding box
		Vertex v[8] = {};

		// Front vertices
		v[0].pos = { minVertex.x, minVertex.y, minVertex.z };
		v[1].pos = { minVertex.x, maxVertex.y, minVertex.z };
		v[2].pos = { maxVertex.x, maxVertex.y, minVertex.z };
		v[3].pos = { maxVertex.x, minVertex.y, minVertex.z };

		// Back vertices
		v[4].pos = { minVertex.x, minVertex.y, maxVertex.z };
		v[5].pos = { minVertex.x, maxVertex.y, maxVertex.z };
		v[6].pos = { maxVertex.x, maxVertex.y, maxVertex.z };
		v[7].pos = { maxVertex.x, minVertex.y, maxVertex.z };

		for (unsigned int i = 0; i < 8; i++)
		{
			boundingBoxVerticesLocal.push_back(v[i]);
		}

		// Indices
		unsigned int indices[36] = {};
		// Front Face
		indices[0] = 0; indices[1] = 1; indices[2] = 3;
		indices[3] = 1; indices[4] = 2; indices[5] = 3;

		// Back Face
		indices[6] = 6; indices[7] = 5; indices[8] = 4;
		indices[9] = 7; indices[10] = 6; indices[11] = 4;

		// Top Face
		indices[12] = 5; indices[13] = 6; indices[14] = 1;
		indices[15] = 1; indices[16] = 6; indices[17] = 2;

		// Bottom Face
		indices[18] = 3; indices[19] = 4; indices[20] = 0;
		indices[21] = 3; indices[22] = 7; indices[23] = 4;

		// Right Face
		indices[24] = 4; indices[25] = 5; indices[26] = 0;
		indices[27] = 5; indices[28] = 1; indices[29] = 0;

		// Left Face
		indices[30] = 3; indices[31] = 2; indices[32] = 7;
		indices[33] = 2; indices[34] = 6; indices[35] = 7;

		for (unsigned int i = 0; i < 36; i++)
		{
			boundingBoxIndicesLocal.push_back(indices[i]);
		}

		m_pBbd = bbp->CreateBoundingBoxData(boundingBoxVerticesLocal, boundingBoxIndicesLocal, m_pPathOfModel);


		return true;
	}
	else
	{
		Log::PrintSeverity(Log::Severity::CRITICAL, "Trying to add a bounding box when no mesh and/or transform exists on entity.\n");
		return false;
	}
}
