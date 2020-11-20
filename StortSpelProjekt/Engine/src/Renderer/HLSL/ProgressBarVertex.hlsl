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

// DH index and VP matrix is stored in here.
ConstantBuffer<CB_PER_OBJECT_STRUCT> cbPerObject : register(b1, space3);

// Used to get the base quad.
StructuredBuffer<vertex> meshes[] : register(t0);

// Camera is stored in here.
ConstantBuffer<CB_PER_FRAME_STRUCT>  cbPerFrame  : register(b4, space3);

// The ProgressBar Specifics is stored in here.
ConstantBuffer<PROGRESS_BAR_DATA> progressBarData : register(b6, space3);

VS_OUT VS_main(uint vID : SV_VertexID)
{
	VS_OUT output = (VS_OUT)0;

	vertex v = meshes[cbPerObject.info.vertexDataIndex][vID];

	float3x3 camSpace = { 
		cbPerFrame.camRight.x, cbPerFrame.camUp.x, cbPerFrame.camForward.x,
		cbPerFrame.camRight.y, cbPerFrame.camUp.y, cbPerFrame.camForward.y,
		cbPerFrame.camRight.z, cbPerFrame.camUp.z, cbPerFrame.camForward.z
	};
	camSpace = transpose(camSpace);

	// v.pos is from -1 -> 1, always 0 on z
	v.pos.x *= progressBarData.maxWidth;
	v.pos.y *= progressBarData.maxHeight;

	// Hack, we only wanna change the 2 vertices on the right
	if (vID == 0 || vID == 2)
	{
		v.pos.x -= progressBarData.maxWidth * 2 * (1.0f - progressBarData.activePercent);
	}

	float3 vertexPosition = progressBarData.position + mul(v.pos, camSpace);

	// Hack to move the back quad further back, to avoid z-fighting
	if (progressBarData.id.x == 0.0f)
	{
		vertexPosition = vertexPosition + cbPerFrame.camForward * 0.03f;
	}

	//							world 					VP
	output.pos = mul(float4(vertexPosition, 1), cbPerObject.WVP);

	output.uv = float2(v.uv.x, v.uv.y);

	return output;
}
