#include "stdafx.h"
#include "BoundingBoxComponent.h"


#include "../Entity.h"

// Creating the BB out of the model
#include "ModelComponent.h"

// Using the same m_pTransform as the models m_pTransform
#include "TransformComponent.h"

#include "../Renderer/BoundingBoxPool.h"

#include "../Renderer/Mesh.h"
#include "../Renderer/ShaderResourceView.h"

#include "../Headers/structs.h"


namespace component
{
	BoundingBoxComponent::BoundingBoxComponent(Entity* parent, bool pick)
		:Component(parent)
	{
		m_CanBePicked = pick;

		m_SlotInfo = new SlotInfo();
	}

	BoundingBoxComponent::~BoundingBoxComponent()
	{
		delete m_SlotInfo;
	}

	void BoundingBoxComponent::Init()
	{
		createBoundingBox();
	}

	void BoundingBoxComponent::Update(double dt)
	{

	}

	void BoundingBoxComponent::SetMesh(Mesh* mesh)
	{
		m_pMesh = mesh;

		m_SlotInfo->vertexDataIndex = mesh->m_pSRV->GetDescriptorHeapIndex();
		// Textures are not used in the WireframeRenderTask
	}

	Transform* BoundingBoxComponent::GetTransform() const
	{
		return m_pTransform;
	}

	const Mesh* BoundingBoxComponent::GetMesh() const
	{
		return m_pMesh;
	}

	const BoundingBoxData* BoundingBoxComponent::GetBoundingBoxData() const
	{
		return m_pBbd;
	}

	const std::string BoundingBoxComponent::GetPathOfModel() const
	{
		return m_pPathOfModel;
	}

	const SlotInfo* BoundingBoxComponent::GetSlotInfo() const
	{
		return m_SlotInfo;
	}


	bool BoundingBoxComponent::CanBePicked() const
	{
		return m_CanBePicked;
	}

	// Writes from BoundingBoxComponent to MeshComponent, which uses this in m_pRenderer
	bool& BoundingBoxComponent::IsPickedThisFrame()
	{
		return m_pParent->GetComponent<ModelComponent>()->m_IsPickedThisFrame;
	}

	bool BoundingBoxComponent::createBoundingBox()
	{
		if (m_pParent->HasComponent<ModelComponent>() == true && m_pParent->HasComponent<TransformComponent>() == true)
		{
			// Use the same m_pTransform as the model
			m_pTransform = m_pParent->GetComponent<TransformComponent>()->GetTransform();
			ModelComponent* mc = m_pParent->GetComponent<ModelComponent>();
			m_pPathOfModel = mc->GetMeshAt(0)->GetPath();
			
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
				std::vector<Vertex> modelVertices = *mc->GetMeshAt(i)->GetVertices();

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
}
