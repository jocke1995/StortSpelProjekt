#include "../../Headers/structs.h"

#define MAX_BONES_PER_VERTEX 10
struct VS_OUT
{
	float4 pos  : SV_Position;
};

struct vertex
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

StructuredBuffer<vertex> meshes[] : register(t0);

VS_OUT VS_main(uint vID : SV_VertexID)
{
	//vertex origVertex			= meshes[cbPerObject.info.textureAlbedo];		// SRV1 orig vertices
	//VertexWeight vertexWeight	= meshes[cbPerObject.info.textureRoughness];	// SRV2 vertexWeights
	//meshes[cbPerObject.info.textureMetallic] = animatedVertex;

	VS_OUT output = (VS_OUT)0;

	vertex mesh = meshes[cbPerObject.info.vertexDataIndex][vID];
	float4 vertexPosition = float4(mesh.pos.xyz, 1.0f);

	output.pos = mul(vertexPosition, cbPerObject.WVP);

	return output;
}