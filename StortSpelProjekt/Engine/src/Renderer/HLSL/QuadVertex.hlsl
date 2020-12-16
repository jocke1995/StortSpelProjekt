#include "../../Headers/structs.h"

struct VS_OUT
{
	float4 pos: SV_POSITION;
	float4 color: COLOR;
	float2 texCoord: TEXCOORD;
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

	vertex v = meshes[cbPerObject.info.vertexDataIndex][vID];
	float4 vertexPosition = float4(v.pos.xyz, 1.0f);

	output.pos = vertexPosition;
	output.color = float4(v.tang, 1.0);
	output.texCoord = float2(v.uv.x, v.uv.y);

	return output;
}