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
StructuredBuffer<float4x4> WVP : register(t0, space1); // Edited by billboard compute

// Testing with getting pos in here.
StructuredBuffer<float4> W : register(t3, space3); // Edited by billboard compute
ConstantBuffer<CB_PER_FRAME_STRUCT>  cbPerFrame  : register(b3, space3);

VS_OUT VS_main(uint vID : SV_VertexID, uint iID : SV_InstanceID)
{
	VS_OUT output = (VS_OUT)0;

	vertex v = meshes[cbPerObject.info.vertexDataIndex][vID];
	float4 vertexPosition = float4(v.pos.xyz, 1.0f);

	float4x4 identity = {
	1, 0, 0, 0,
	0, 1, 0, 0,
	0, 0, 1, 0,
	0, 0, 0, 1
	};
	
	float4x4 sizeMat = W[iID].a * identity;
	float4x4 transMat = {
	1, 0, 0, W[iID].x,
	0, 1, 0, W[iID].y,
	0, 0, 1, W[iID].z,
	0, 0, 0, 1
	};

	float4x4 WorldMat = transpose(mul(transMat, sizeMat));
	float4x4 WVP = mul(cbPerObject.WVP, WorldMat);

	output.pos = mul(vertexPosition, WVP);

	output.uv = float2(v.uv.x, v.uv.y);

	return output;
}