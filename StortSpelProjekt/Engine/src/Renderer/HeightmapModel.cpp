#include "stdafx.h"
#include "HeightmapModel.h"

HeightmapModel::HeightmapModel(const std::wstring* path, std::vector<Mesh*>* meshes, std::vector<Animation*>* animations, std::vector<Material*>* materials, float* heights) : 
	Model(path, meshes, animations, materials),
	m_pHeights(heights)
{
}

HeightmapModel::~HeightmapModel()
{
	if (m_pHeights)
	{
		delete m_pHeights;
	}
}

const float* HeightmapModel::GetHeights()
{
	return m_pHeights;
}
