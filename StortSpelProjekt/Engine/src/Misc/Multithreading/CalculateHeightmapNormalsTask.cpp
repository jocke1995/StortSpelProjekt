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

	float3 tangent;
	float3 biTangent;
	for (unsigned int i = lowestVertexId; i < highestVertexId; i++)
	{
		tangent = { 0,0,1 };
		vertexNormal = { m_pVertices[i].normal.x , m_pVertices[i].normal.y, m_pVertices[i].normal.z };
		biTangent = tangent.cross(&vertexNormal);
		tangent = vertexNormal.cross(&biTangent);

		m_pVertices[i].tangent.x = tangent.x;
		m_pVertices[i].tangent.y = tangent.y;
		m_pVertices[i].tangent.z = tangent.z;
	}
}
