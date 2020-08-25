#include "../../Headers/structs.h"

struct VS_OUT
{
	float4 pos      : SV_Position;
	float4 worldPos : WPos;
	float4 uv       : UV;
	float3x3 tbn	: TBN;
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

	vertex v = meshes[cbPerObject.info.vertexDataIndex][vID];
	float4 vertexPosition = float4(v.pos.xyz, 1.0f);

	output.pos = mul(vertexPosition, cbPerObject.WVP);
	output.worldPos = mul(vertexPosition, cbPerObject.worldMatrix);

	output.uv = float4(v.uv);
	
	// Create TBN-Matrix
	float3 T = normalize(mul(float4(v.tang), cbPerObject.worldMatrix)).xyz;
	float3 N = normalize(mul(float4(v.norm), cbPerObject.worldMatrix)).xyz;

	// Gram schmidt
	T = normalize(T - dot(T, N) * N);

	float3 B = cross(T, N);

	output.tbn = float3x3(T, B, N);

	return output;
}