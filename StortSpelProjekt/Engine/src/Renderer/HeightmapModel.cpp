#include "stdafx.h"
#include "HeightmapModel.h"

HeightmapModel::HeightmapModel(const std::wstring* path, std::vector<Mesh*>* meshes, std::vector<Material*>* materials, double* heights, double width, double length) : 
	Model(path, meshes, materials),
	m_pHeights(heights),
	m_Width(width),
	m_Length(length)
{
}

HeightmapModel::~HeightmapModel()
{
	if (m_pHeights)
	{
		delete m_pHeights;
	}
}

const double* HeightmapModel::GetHeights()
{
	return m_pHeights;
}

const double HeightmapModel::GetWidth()
{
	return m_Width;
}

const double HeightmapModel::GetLength()
{
	return m_Length;
}
