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
StructuredBuffer<PARTICLE_DATA> particleData : register(t3, space3); // Edited by billboard compute
ConstantBuffer<CB_PER_FRAME_STRUCT>  cbPerFrame  : register(b4, space3);

VS_OUT VS_main(uint vID : SV_VertexID, uint iID : SV_InstanceID)
{
	VS_OUT output = (VS_OUT)0;

	float halfSize = (particleData[iID].size)/2;

	vertex v = meshes[cbPerObject.info.vertexDataIndex][vID];

	float3x3 camSpace = { 
		cbPerFrame.camRight.x, cbPerFrame.camUp.x, cbPerFrame.camForward.x,
		cbPerFrame.camRight.y, cbPerFrame.camUp.y, cbPerFrame.camForward.y,
		cbPerFrame.camRight.z, cbPerFrame.camUp.z, cbPerFrame.camForward.z
	};
	camSpace = transpose(camSpace);

	float3x3 rotMatrix = {
		cos(particleData[iID].rotation), -sin(particleData[iID].rotation), 0,
		sin(particleData[iID].rotation), cos(particleData[iID].rotation), 0,
		0, 0, 1
	};
	rotMatrix = transpose(rotMatrix);

	// v.pos is from -1 -> 1, always 0 on z
	float3 vertexPosition = particleData[iID].position + mul(v.pos * halfSize, mul(rotMatrix, camSpace));
	
	//							world 					VP
	output.pos = mul(float4(vertexPosition, 1), cbPerObject.WVP);

	output.uv = float2(v.uv.x, v.uv.y);

	return output;
}
