#include "../../Headers/structs.h"

struct VS_OUT
{
	float4 pos  : SV_Position;
};

struct vertex
{
	float4 pos;
	float4 uv;
	float4 norm;
	float4 tang;
};

ConstantBuffer<CB_PER_OBJECT_STRUCT> cbPerObject : register(b1, space3);

StructuredBuffer<vertex> meshes[] : register(t0);

VS_OUT VS_main(uint vID : SV_VertexID)
{
	VS_OUT output = (VS_OUT)0;

	vertex mesh = meshes[cbPerObject.info.vertexDataIndex][vID];
	float4 vertexPosition = float4(mesh.pos.xyz, 1.0f);

	output.pos = mul(vertexPosition, cbPerObject.WVP);

	return output;
}