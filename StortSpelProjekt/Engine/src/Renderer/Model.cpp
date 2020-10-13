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
