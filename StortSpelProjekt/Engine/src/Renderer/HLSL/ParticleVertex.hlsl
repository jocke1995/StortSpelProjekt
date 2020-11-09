#include "../../Headers/structs.h"

struct VS_OUT
{
	float4 pos      : SV_Position;
	float2 uv       : UV;
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
StructuredBuffer<float4x4> uavWMat : register(t3, space3); // Edited by billboard compute


VS_OUT VS_main(uint vID : SV_VertexID, uint iID : SV_InstanceID)
{
	VS_OUT output = (VS_OUT)0;

	vertex v = meshes[cbPerObject.info.vertexDataIndex][vID];
	float4 vertexPosition = float4(v.pos.xyz, 1.0f);

	float4x4 a = uavWMat[0];
	output.pos = mul(vertexPosition, a);

	output.uv = float2(v.uv.x, v.uv.y);

	return output;
}