#include "BoundingBoxComponent.h"

namespace component
{
	BoundingBoxComponent::BoundingBoxComponent(Entity* parent, bool pick, bool outlineWhenPicked)
		:Component(parent)
	{
		this->pick = pick;
		this->outlineWhenPicked = outlineWhenPicked;
	}

	BoundingBoxComponent::~BoundingBoxComponent()
	{
		
	}

	void BoundingBoxComponent::Init()
	{
		this->CreateBoundingBox();
	}

	void BoundingBoxComponent::Update(double dt)
	{

	}

	void BoundingBoxComponent::SetMesh(Mesh* mesh)
	{
		this->mesh = mesh;
	}

	Transform* BoundingBoxComponent::GetTransform() const
	{
		return this->transform;
	}

	const Mesh* BoundingBoxComponent::GetMesh() const
	{
		return this->mesh;
	}

	const BoundingBoxData* BoundingBoxComponent::GetBoundingBoxData() const
	{
		return this->bbd;
	}

	const std::string BoundingBoxComponent::GetPathOfModel() const
	{
		return this->pathOfModel;
	}

	std::string BoundingBoxComponent::GetParentName() const
	{
		std::string parentName = this->parent->GetName();
		return parentName;
	}

	bool BoundingBoxComponent::Pick() const
	{
		return this->pick;
	}

	bool BoundingBoxComponent::Outline() const
	{
		return this->outlineWhenPicked;
	}

	bool BoundingBoxComponent::CreateBoundingBox()
	{
		if (this->parent->HasComponent<MeshComponent>() == true && this->parent->HasComponent<TransformComponent>() == true)
		{
			// Use the same transform as the model
			this->transform = this->parent->GetComponent<TransformComponent>()->GetTransform();
			MeshComponent* mc = this->parent->GetComponent<MeshComponent>();
			this->pathOfModel = mc->GetMesh(0)->GetPath();

			BoundingBoxPool* bbp = BoundingBoxPool::Get();
			if (bbp->BoundingBoxDataExists(this->pathOfModel) == true)
			{
				this->bbd = bbp->GetBoundingBoxData(this->pathOfModel);
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

			// Create bounding box
			Vertex v[8] = {};

			// Front vertices
			v[0].pos = { minVertex.x, minVertex.y, minVertex.z, 1.0f};
			v[1].pos = { minVertex.x, maxVertex.y, minVertex.z, 1.0f };
			v[2].pos = { maxVertex.x, maxVertex.y, minVertex.z, 1.0f };
			v[3].pos = { maxVertex.x, minVertex.y, minVertex.z, 1.0f };

			// Back vertices
			v[4].pos = { minVertex.x, minVertex.y, maxVertex.z, 1.0f };
			v[5].pos = { minVertex.x, maxVertex.y, maxVertex.z, 1.0f };
			v[6].pos = { maxVertex.x, maxVertex.y, maxVertex.z, 1.0f };
			v[7].pos = { maxVertex.x, minVertex.y, maxVertex.z, 1.0f };

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

			this->bbd = bbp->CreateBoundingBoxData(boundingBoxVerticesLocal, boundingBoxIndicesLocal, this->pathOfModel);
			
			return true;
		}
		else
		{
			Log::PrintSeverity(Log::Severity::CRITICAL, "Trying to add a bounding box when no mesh and/or transform exists on entity.\n");
			return false;
		}
	}
}
