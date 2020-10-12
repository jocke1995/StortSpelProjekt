#include "stdafx.h"
#include "CalculateHeightmapNormalsTask.h"
#include "../Renderer/Mesh.h"
CalculateHeightmapNormalsTask::CalculateHeightmapNormalsTask(unsigned int id, unsigned int nrOfThreads, std::vector<Vertex>& vertices, std::vector<unsigned int>& indices, unsigned int width, unsigned int height) :
	MultiThreadedTask(FLAG_THREAD::HEIGHTMAPLOAD),
	m_Id(id),
	m_NrOfThreads(nrOfThreads),
	m_pVertices(vertices),
	m_pIndices(indices),
	m_Width(width),
	m_Height(height)
{
}

void CalculateHeightmapNormalsTask::Execute()
{
	unsigned int verticesPerThread = m_pVertices.size() / m_NrOfThreads;
	unsigned int lowestVertexId = m_Id * verticesPerThread;
	unsigned int highestVertexId = (1 + m_Id) * verticesPerThread;

	// Calculate normals
	float3 neighbours[4];
	float3 neighbourTangents[4];
	float3 normals[4];
	float3 verPos;
	float3 vertexNormal;
	// left = 0
	// down = 1
	// right = 2
	// up = 3

	for (unsigned int i = lowestVertexId; i < highestVertexId; i++)
	{
		verPos = { m_pVertices[i].pos.x, m_pVertices[i].pos.y, m_pVertices[i].pos.z };
		// Is left in the grid?
		if (i != 0)
		{
			neighbours[0] = { m_pVertices[i - 1].pos.x, m_pVertices[i - 1].pos.y, m_pVertices[i - 1].pos.z };
		}
		else
		{
			neighbours[0] = { 0,0,0 };
		}

		// is down on the grid?
		if (i / m_Width + 1 < m_Height)
		{
			neighbours[1] = { m_pVertices[i + m_Width].pos.x, m_pVertices[i + m_Width].pos.y, m_pVertices[i + m_Width].pos.z };
		}
		else
		{
			neighbours[1] = { 0,0,0 };
		}

		// Is right on the grid?
		if (i % m_Width + 1 < m_Height)
		{
			neighbours[2] = { m_pVertices[i + 1].pos.x, m_pVertices[i + 1].pos.y, m_pVertices[i + 1].pos.z };
		}
		else
		{
			neighbours[2] = { 0,0,0 };
		}

		// Is up on the grid?
		if (i >= m_Width)
		{
			neighbours[3] = { m_pVertices[i - m_Width].pos.x, m_pVertices[i - m_Width].pos.y, m_pVertices[i - m_Width].pos.z };
		}
		else
		{
			neighbours[3] = { 0,0,0 };
		}

		// Neighbours calculated. Calculate normals and store them.

		neighbourTangents[0] = neighbours[0] - verPos;
		neighbourTangents[1] = neighbours[1] - verPos;
		neighbourTangents[2] = neighbours[2] - verPos;
		neighbourTangents[3] = neighbours[3] - verPos;

		normals[0] = neighbourTangents[1].cross(&neighbourTangents[0]);
		normals[1] = neighbourTangents[2].cross(&neighbourTangents[1]);
		normals[2] = neighbourTangents[3].cross(&neighbourTangents[2]);
		normals[3] = neighbourTangents[0].cross(&neighbourTangents[3]);

		vertexNormal = normals[0] + normals[1] + normals[2] + normals[3];
		vertexNormal.normalize();

		m_pVertices[i].normal.x = -vertexNormal.x;
		m_pVertices[i].normal.y = -vertexNormal.y;
		m_pVertices[i].normal.z = -vertexNormal.z;
	}


	verticesPerThread = (m_pIndices.size() - 3) / m_NrOfThreads;
	lowestVertexId = m_Id * verticesPerThread;
	highestVertexId = (1 + m_Id) * verticesPerThread;
	// Move on to tangents.
	float3 edge1 = { 0,0,0 };
	float3 edge2 = { 0,0,0 };
	float2 deltaUV1 = { 0,0 };
	float2 deltaUV2 = { 0,0 };
	//Reuse neighbours array from normals!
	neighbours[0] = { 0 };
	neighbours[1] = { 0 };
	neighbours[2] = { 0 };
	float2 uv[3] = { 0 };
	float f = 0;

	for (unsigned int i = lowestVertexId; i < highestVertexId; i++)
	{
		neighbours[0] = { m_pVertices[m_pIndices[i]].pos.x,	  m_pVertices[m_pIndices[i]].pos.y,	  m_pVertices[m_pIndices[i]].pos.z };
		neighbours[1] = { m_pVertices[m_pIndices[i + 1]].pos.x, m_pVertices[m_pIndices[i + 1]].pos.y, m_pVertices[m_pIndices[i + 1]].pos.z };
		neighbours[2] = { m_pVertices[m_pIndices[i + 2]].pos.x, m_pVertices[m_pIndices[i + 2]].pos.y, m_pVertices[m_pIndices[i + 2]].pos.z };

		uv[0] = { m_pVertices[m_pIndices[i]].uv.x,	 m_pVertices[m_pIndices[i]].uv.y };
		uv[1] = { m_pVertices[m_pIndices[i + 1]].uv.x, m_pVertices[m_pIndices[i + 1]].uv.y };
		uv[2] = { m_pVertices[m_pIndices[i + 2]].uv.x, m_pVertices[m_pIndices[i + 2]].uv.y };

		edge1 = neighbours[1] - neighbours[0];
		edge2 = neighbours[2] - neighbours[0];
		deltaUV1 = uv[1] - uv[0];
		deltaUV2 = uv[2] - uv[0];

		f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);
		for (unsigned int j = 0; j < 3; j++)
		{
			m_pVertices[m_pIndices[i + j]].tangent.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
			m_pVertices[m_pIndices[i + j]].tangent.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
			m_pVertices[m_pIndices[i + j]].tangent.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
		}
	}
}
