#include "stdafx.h"
#include "NavMesh.h"

NavMesh::NavMesh()
{
}

void NavMesh::AddNavQuad(float3 position, float2 size)
{
	NavQuad temp;
	temp.position = position;
	temp.size = size;
	m_NavQuads.push_back(temp);
}