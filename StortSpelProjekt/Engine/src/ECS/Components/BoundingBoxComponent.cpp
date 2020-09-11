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
	// Making a temporary OBB which takes the original state of the OBB
	DirectX::BoundingOrientedBox obb;
	obb.Center = m_OriginalBoundingBox.Center;
	obb.Extents = m_OriginalBoundingBox.Extents;
	obb.Orientation = m_OriginalBoundingBox.Orientation;

	// then do all the transformations on this temoporary OBB so we don't change the original state
	obb.Transform(obb, *m_pTransform->GetWorldMatrix());

	// now save the transformations to the OBB that is used in collision detection
	m_OrientedBoundingBox.Center = obb.Center;
	m_OrientedBoundingBox.Extents = obb.Extents;
	m_OrientedBoundingBox.Orientation = obb.Orientation;	
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
	return m_PathOfModel;
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
		m_PathOfModel = mc->GetMesh(0)->GetPath();

		BoundingBoxPool* bbp = BoundingBoxPool::Get();
		if (bbp->BoundingBoxDataExists(m_PathOfModel) == true)
		{
			m_pBbd = bbp->GetBoundingBoxData(m_PathOfModel);
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
		float3 absHalfLenghtOfRect = { (abs(minVertex.x) + abs(maxVertex.x)) / 2 ,
										(abs(minVertex.y) + abs(maxVertex.y)) / 2 ,
										(abs(minVertex.z) + abs(maxVertex.z)) / 2 };
		m_OrientedBoundingBox.Extents.x = absHalfLenghtOfRect.x;
		m_OrientedBoundingBox.Extents.y = absHalfLenghtOfRect.y;
		m_OrientedBoundingBox.Extents.z = absHalfLenghtOfRect.z;

		// Set the position of the OBB
		m_OrientedBoundingBox.Center.x = 0;
		// y pos of models should be at 0, and we want the center of the model, so add the distance to center
		m_OrientedBoundingBox.Center.y = ((minVertex.y) + (maxVertex.y)) / 2; 
		m_OrientedBoundingBox.Center.z = 0;

		// get roataion from the rotationMatrix and convert it 
		// to the required quaternion(XMFLOAT4) and store it
		DirectX::XMMATRIX wm = m_pTransform->GetRotMatrix();
		DirectX::XMVECTOR qv = DirectX::XMQuaternionRotationMatrix(wm);
		DirectX::XMStoreFloat4(&m_OrientedBoundingBox.Orientation, qv);

		// save this original state of the boundingBox so that we can apply the correct math in update()
		m_OriginalBoundingBox = m_OrientedBoundingBox;

		// Saving down OBB corners
		DirectX::XMFLOAT3 corners[8];
		m_OrientedBoundingBox.GetCorners(corners);

		// Create the drawn bounding box
		Vertex v[8] = {};

		// The vertices are the corners of the OBB so send them
		// Front vertices
		v[0].pos = corners[0];
		v[1].pos = corners[1];
		v[2].pos = corners[2];
		v[3].pos = corners[3];
	
		// Back vertices
		v[4].pos = corners[4];
		v[5].pos = corners[5];
		v[6].pos = corners[6];
		v[7].pos = corners[7];


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

		m_pBbd = bbp->CreateBoundingBoxData(boundingBoxVerticesLocal, boundingBoxIndicesLocal, m_PathOfModel);


		return true;
	}
	else
	{
		Log::PrintSeverity(Log::Severity::CRITICAL, "Trying to add a bounding box when no mesh and/or transform exists on entity.\n");
		return false;
	}

}
