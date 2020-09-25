#include "stdafx.h"
#include "ModelComponent.h"

#include "../Renderer/Model.h"
#include "../Renderer/Material.h"
#include "../Renderer/Renderer.h"
#include "../Entity.h"

namespace component
{
	ModelComponent::ModelComponent(Entity* parent)
		:Component(parent)
	{
	}

	ModelComponent::~ModelComponent()
	{
		
	}

	void ModelComponent::SetModel(Model* model)
	{
		m_Model = model;
	}

	void ModelComponent::SetDrawFlag(unsigned int drawFlag)
	{
		m_DrawFlag = drawFlag;
	}

	void ModelComponent::Update(double dt)
	{

	}

	void ModelComponent::InitScene()
	{
		Log::Print("ModelComponent InitScene called!\n");
		// check if model has transform component
		component::TransformComponent* tc = GetParent()->GetComponent<component::TransformComponent>();
		if (tc != nullptr)
		{
			Renderer::GetInstance().InitModelComponent(GetParent());
		}
	}

	Mesh* ModelComponent::GetMeshAt(unsigned int index) const
	{
		return m_Model->GetMeshAt(index);
	}

	Material* ModelComponent::GetMaterialAt(unsigned int index) const
	{
		return m_Model->GetMaterialAt(index);
	}

	const SlotInfo* ModelComponent::GetSlotInfoAt(unsigned int index) const
	{
		return m_Model->GetSlotInfoAt(index);
	}

	unsigned int ModelComponent::GetDrawFlag() const
	{
		return m_DrawFlag;
	}

	unsigned int ModelComponent::GetNrOfMeshes() const
	{
		return m_Model->GetSize();
	}
	bool ModelComponent::IsPickedThisFrame() const
	{
		return m_IsPickedThisFrame;
	}
}
