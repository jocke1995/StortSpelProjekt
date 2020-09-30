#include "stdafx.h"
#include "Model.h"
#include "Mesh.h"
#include "Material.h"
#include "Texture/Texture2D.h"
#include "GPUMemory/ShaderResourceView.h"
#include "structs.h"
#include "Animation.h"

Model::Model(const std::wstring* path, std::vector<Mesh*>* meshes, std::vector<Animation*>* animations, std::vector<Material*>* materials)
{
	m_Path = *path;
	m_Size = (*meshes).size();

	m_Meshes = (*meshes);
	m_Animations = (*animations);
	m_Materials = (*materials);

	// Fill slotinfo with empty slotinfos
	m_SlotInfos.resize(m_Size);
}

Model::~Model()
{
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
