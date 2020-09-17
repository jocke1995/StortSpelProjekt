#include "stdafx.h"
#include "Model.h"
#include "Mesh.h"
#include "Texture.h"
#include "ShaderResourceView.h"
#include "structs.h"
#include "Animation.h"

Model::Model(const std::wstring path, std::vector<Mesh*>* meshes, std::vector<Animation*>* animations, std::vector<std::map<TEXTURE_TYPE, Texture*>>* textures)
{
	m_Path = path;
	m_Size = (*meshes).size();

	m_Meshes = (*meshes);
	m_Animations = (*animations);
	m_Textures = (*textures);

	// Fill SlotInfo with mesh+material info
	for (unsigned int i = 0; i < (*meshes).size(); i++)
	{
		m_SlotInfos.push_back(
			{
			(*meshes)[i]->m_pSRV->GetDescriptorHeapIndex(),
			(*textures)[i][TEXTURE_TYPE::AMBIENT]->GetDescriptorHeapIndex(),
			(*textures)[i][TEXTURE_TYPE::DIFFUSE]->GetDescriptorHeapIndex(),
			(*textures)[i][TEXTURE_TYPE::SPECULAR]->GetDescriptorHeapIndex(),
			(*textures)[i][TEXTURE_TYPE::NORMAL]->GetDescriptorHeapIndex(),
			(*textures)[i][TEXTURE_TYPE::EMISSIVE]->GetDescriptorHeapIndex(),
			});
	}
}

Model::~Model()
{
}

std::wstring Model::GetPath() const
{
	return m_Path;
}

unsigned int Model::GetSize() const
{
	return m_Size;
}

Mesh* Model::GetMeshAt(unsigned int index)
{
	return m_Meshes[index];
}

std::map<TEXTURE_TYPE, Texture*>* Model::GetTexturesAt(unsigned int index)
{
	return &m_Textures[index];
}

SlotInfo* Model::GetSlotInfoAt(unsigned int index)
{
	return &m_SlotInfos[index];
}
