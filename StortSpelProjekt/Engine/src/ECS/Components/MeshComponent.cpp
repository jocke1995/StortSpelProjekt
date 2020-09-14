#include "stdafx.h"
#include "MeshComponent.h"
#include "../Renderer/Mesh.h"
namespace component
{
	MeshComponent::MeshComponent(Entity* parent)
		:Component(parent)
	{

	}

	MeshComponent::~MeshComponent()
	{
		
	}

	void MeshComponent::SetMeshes(std::vector<Mesh*>* meshes)
	{
		for (Mesh* mesh : *meshes)
		{
			m_Meshes.push_back(mesh);
		}
	}

	void MeshComponent::SetDrawFlag(unsigned int drawFlag)
	{
		m_DrawFlag = drawFlag;
	}

	void MeshComponent::Update(double dt)
	{

	}

	Mesh* MeshComponent::GetMesh(unsigned int index) const
	{
		return m_Meshes[index];
	}

	unsigned int MeshComponent::GetDrawFlag() const
	{
		return m_DrawFlag;
	}

	unsigned int MeshComponent::GetNrOfMeshes() const
	{
		return m_Meshes.size();
	}
	bool MeshComponent::IsPickedThisFrame() const
	{
		return m_IsPickedThisFrame;
	}
}
