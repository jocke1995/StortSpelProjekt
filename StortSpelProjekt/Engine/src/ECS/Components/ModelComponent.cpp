#include "stdafx.h"
#include "ModelComponent.h"
#include "../Renderer/Model.h"
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

	void ModelComponent::Init(Renderer* renderer)
	{
		Log::Print("ModelComponent Init called!\n");
	}

	Mesh* ModelComponent::GetMeshAt(unsigned int index) const
	{
		return m_Model->GetMeshAt(index);
	}

	std::map<TEXTURE_TYPE, Texture*>* ModelComponent::GetTexturesAt(unsigned int index) const
	{
		return m_Model->GetTexturesAt(index);
	}

	SlotInfo* ModelComponent::GetSlotInfoAt(unsigned int index) const
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
