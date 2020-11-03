#include "../../Headers/structs.h"

struct VS_OUT
{
	float4 pos  : SV_Position;
};

struct Vertex
{
	float3 pos;
	float2 uv;
	float3 norm;
	float3 tang;
};

struct VertexWeight
{
	unsigned int boneIDs[MAX_BONES_PER_VERTEX];
	float weights[MAX_BONES_PER_VERTEX];
};

ConstantBuffer<CB_PER_OBJECT_STRUCT> cbPerObject	: register(b1, space3);
ConstantBuffer<DescriptorHeapIndices> indexConstants : register(b2, space3);

StructuredBuffer<Vertex> vertices[]				: register(t0, space0);
StructuredBuffer<VertexWeight> vertexWeights[]	: register(t0, space1);

RWStructuredBuffer<Vertex> verticesUAV[] : register(u0);

// Matrices
ConstantBuffer<ANIMATION_MATRICES_STRUCT> animationMatrices  : register(b6, space3);

// Helper functions
Vertex AnimateVertex(Vertex origVertex, VertexWeight vertexWeight);

VS_OUT VS_main(uint vID : SV_VertexID)
{
	// SRV1 orig vertices
	Vertex origVertex			= vertices[indexConstants.index0][vID];
	// SRV2 vertexWeights
	VertexWeight vertexWeight	= vertexWeights[indexConstants.index1][vID];

	// UAV1 Write modified vertices, these vertices will henceforth be used in rendering passes each frame
	Vertex transformedVertex = AnimateVertex(origVertex, vertexWeight);
	verticesUAV[indexConstants.index2][vID] = transformedVertex;

	// Continue as usual with depth pre-pass with new transformed vertices
	VS_OUT output = (VS_OUT)0;

	Vertex mesh = transformedVertex;
	float4 vertexPosition = float4(mesh.pos.xyz, 1.0f);

	output.pos = mul(vertexPosition, cbPerObject.WVP);

	return output;
}

Vertex AnimateVertex(Vertex vertex, VertexWeight vertexWeight)
{
	Vertex animatedVertex = vertex;

	float3 skinnedPosL = 0;
	float3 skinnedNormalL = 0;
	[loop] for (int i = 0; i < MAX_BONES_PER_VERTEX; i++)
	{
		if (vertexWeight.weights[i] > 0)
		{
			// For each bone affecting the vertex, transform the vertex and normal according to the matrix of the bone and the influence (weight).
			skinnedPosL += mul(float4(vertex.pos, 1), animationMatrices.matrices[vertexWeight.boneIDs[i]]).xyz * vertexWeight.weights[i];
			skinnedNormalL += mul(float4(vertex.norm, 0), animationMatrices.matrices[vertexWeight.boneIDs[i]]).xyz * vertexWeight.weights[i];
		}
	}

	animatedVertex.pos = skinnedPosL;
	animatedVertex.norm = skinnedNormalL;

	return animatedVertex;
}
