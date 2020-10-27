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

ConstantBuffer<CB_PER_OBJECT_STRUCT> cbPerObject : register(b1, space3);

StructuredBuffer<Vertex> vertices[] : register(t0, space0);
StructuredBuffer<VertexWeight> vertexWeights[] : register(t0, space1);

RWStructuredBuffer<Vertex> verticesUAV[] : register(u0);

// Matrices
ConstantBuffer<ANIMATION_MATRICES_STRUCT> animationMatrices  : register(b3, space3);

matrix Identity =
{
	{ 1, 0, 0, 0 },
	{ 0, 1, 0, 0 },
	{ 0, 0, 1, 0 },
	{ 0, 0, 0, 1 }
};

// Helper functions
Vertex AnimateVertex(Vertex origVertex, VertexWeight vertexWeight);

VS_OUT VS_main(uint vID : SV_VertexID)
{
	// SRV1 orig vertices
	Vertex origVertex			= vertices[cbPerObject.info.textureAlbedo][vID];
	// SRV2 vertexWeights
	VertexWeight vertexWeight	= vertexWeights[cbPerObject.info.textureRoughness][vID];

	// UAV1 Write modified vertices, these vertices will henceforth be used in rendering passes each frame
	Vertex transformedVertex = AnimateVertex(origVertex, vertexWeight);
	verticesUAV[cbPerObject.info.textureMetallic][vID] = transformedVertex;

	// Continue as usual with depth pre-pass with new transformed vertices
	VS_OUT output = (VS_OUT)0;

	Vertex mesh = transformedVertex;
	float4 vertexPosition = float4(mesh.pos.xyz, 1.0f);

	output.pos = mul(vertexPosition, cbPerObject.WVP);

	return output;
}

Vertex AnimateVertex(Vertex vertex, VertexWeight vertexWeight)
{
	Vertex animatedVertex;
	float4x4 animationTransform;
	
	//animationTransform = Identity;
	animationTransform  = animationMatrices.matrices[vertexWeight.boneIDs[0]] * vertexWeight.weights[0];
	animationTransform += animationMatrices.matrices[vertexWeight.boneIDs[1]] * vertexWeight.weights[1];
	animationTransform += animationMatrices.matrices[vertexWeight.boneIDs[2]] * vertexWeight.weights[2];
	animationTransform += animationMatrices.matrices[vertexWeight.boneIDs[3]] * vertexWeight.weights[3];
	animationTransform += animationMatrices.matrices[vertexWeight.boneIDs[4]] * vertexWeight.weights[4];
	animationTransform += animationMatrices.matrices[vertexWeight.boneIDs[5]] * vertexWeight.weights[5];
	animationTransform += animationMatrices.matrices[vertexWeight.boneIDs[6]] * vertexWeight.weights[6];
	animationTransform += animationMatrices.matrices[vertexWeight.boneIDs[7]] * vertexWeight.weights[7];
	animationTransform += animationMatrices.matrices[vertexWeight.boneIDs[8]] * vertexWeight.weights[8];
	animationTransform += animationMatrices.matrices[vertexWeight.boneIDs[9]] * vertexWeight.weights[9];

	float4 animatedPosition = mul(float4(vertex.pos, 1.0f), animationTransform);

	//float4 animatedPosition = { 1.0f,1.0f,1.0f,1.0f };
	animatedVertex.pos = animatedPosition.xyz;
	animatedVertex.uv = vertex.uv;
	animatedVertex.norm = vertex.norm;
	animatedVertex.tang = vertex.tang;
	//animatedVertex.pos = float3(animatedPosition.xyz);
	return animatedVertex;
}
