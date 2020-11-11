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
StructuredBuffer<float4> particlePos : register(t3, space3); // Edited by billboard compute
ConstantBuffer<CB_PER_FRAME_STRUCT>  cbPerFrame  : register(b3, space3);

VS_OUT VS_main(uint vID : SV_VertexID, uint iID : SV_InstanceID)
{
	VS_OUT output = (VS_OUT)0;

	float halfSize = (particlePos[iID].a)/2;

	vertex v = meshes[cbPerObject.info.vertexDataIndex][vID];

	float3x3 camSpace = { 
		cbPerFrame.camRight.x, cbPerFrame.camUp.x, cbPerFrame.camForward.x,
		cbPerFrame.camRight.y, cbPerFrame.camUp.y, cbPerFrame.camForward.y,
		cbPerFrame.camRight.z, cbPerFrame.camUp.z, cbPerFrame.camForward.z
	};

	float3 vertexPosition = particlePos[iID].xyz + mul(v.pos * halfSize, transpose(camSpace));
	
	output.pos = mul(float4(vertexPosition, 1), cbPerObject.WVP);

	output.uv = float2(v.uv.x, v.uv.y);

	return output;
}