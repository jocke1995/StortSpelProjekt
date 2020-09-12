#include "../../Headers/structs.h"

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

ConstantBuffer<CB_PER_OBJECT_STRUCT> cbPerObject : register(b1, space3);

StructuredBuffer<vertex> meshes[] : register(t0);

VS_OUT VS_main(uint vID : SV_VertexID)
{
	VS_OUT output = (VS_OUT)0;

	vertex mesh = meshes[cbPerObject.info.vertexDataIndex][vID];
	output.pos = float4(mesh.pos.xyz, 1.0f);

	return output;
}