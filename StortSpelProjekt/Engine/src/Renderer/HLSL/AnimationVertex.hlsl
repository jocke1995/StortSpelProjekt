#include "../../Headers/structs.h"

struct VS_OUT
{
	float4 pos				: SV_Position;
	float4 worldPos			: WPos;
	float2 uv				: UV;
	float3x3 tbn			: TBN;
	unsigned int boneIds[10]: IDS;
	float weights[10]		: WEIGHTS;
};

struct animatedVertex
{
	float3 pos;
	float2 uv;
	float3 norm;
	float3 tang;
	unsigned int boneIds[10];
	float weights[10];
};

ConstantBuffer<CB_PER_OBJECT_STRUCT> cbPerObject : register(b1, space3);
StructuredBuffer<animatedVertex> meshes[] : register(t0);

VS_OUT VS_main(uint vID : SV_VertexID)
{
	VS_OUT output = (VS_OUT)0;

	animatedVertex v = meshes[cbPerObject.info.vertexDataIndex][vID];
	float4 vertexPosition = float4(v.pos.xyz, 1.0f);

	output.pos = mul(vertexPosition, cbPerObject.WVP);
	output.worldPos = mul(vertexPosition, cbPerObject.worldMatrix);

	output.uv = float2(v.uv.x, v.uv.y);

	// Create TBN-Matrix
	float3 T = normalize(mul(float4(v.tang, 0.0f), cbPerObject.worldMatrix)).xyz;
	float3 N = normalize(mul(float4(v.norm, 0.0f), cbPerObject.worldMatrix)).xyz;

	// Gram schmidt
	T = normalize(T - dot(T, N) * N);

	float3 B = cross(T, N);

	output.tbn = float3x3(T, B, N);

	return output;
}