#include "../../Headers/structs.h"

#define MAX_BONES_PER_VERTEX 10
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

StructuredBuffer<Vertex> vertices[] : register(t0);
StructuredBuffer<VertexWeight> vertexWeights[] : register(t0);

RWStructuredBuffer<Vertex> verticesUAV[] : register(u0);

Vertex AnimateVertex(Vertex origVertex, VertexWeight vertexWeight);

VS_OUT VS_main(uint vID : SV_VertexID)
{
	// SRV1 orig vertices
	Vertex origVertex			= vertices[cbPerObject.info.textureAlbedo][vID];	
	// SRV2 vertexWeights
	VertexWeight vertexWeight	= vertexWeights[cbPerObject.info.textureRoughness][vID];	

	// UAV1 Write modified vertices
	Vertex transformedVertex = AnimateVertex(origVertex, vertexWeight);
	verticesUAV[cbPerObject.info.textureMetallic][vID] = transformedVertex;

	// Continue as usual with depth pre-pass
	VS_OUT output = (VS_OUT)0;

	Vertex mesh = transformedVertex;
	float4 vertexPosition = float4(mesh.pos.xyz, 1.0f);

	output.pos = mul(vertexPosition, cbPerObject.WVP);

	return output;
}

// Todo: Write animation in here
Vertex AnimateVertex(Vertex origVertex, VertexWeight vertexWeight)
{
	return origVertex;
}
