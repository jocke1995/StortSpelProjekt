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
ConstantBuffer<CB_PER_GUI_STRUCT> cbGUIData : register(b6, space3);

StructuredBuffer<vertex> meshes[] : register(t0);

VS_OUT VS_main(uint vID : SV_VertexID)
{
	VS_OUT output = (VS_OUT)0;

	vertex v = meshes[cbPerObject.info.vertexDataIndex][vID];

	// Based on indices order
	float4 vertexPosition;
	float4 v0 = float4(cbGUIData.pos.x, cbGUIData.pos.y, 0.0f, 1.0f);
	float4 v1 = float4(cbGUIData.pos.x, cbGUIData.scale.y, 0.0f, 1.0f);
	float4 v2 = float4(cbGUIData.scale.x, cbGUIData.pos.y, 0.0f, 1.0f);
	float4 v3 = float4(cbGUIData.scale.x, cbGUIData.scale.y, 0.0f, 1.0f);
	//float4 v0 = float4(0.0f, 0.0f, 0.0f, 1.0f);
	//float4 v1 = float4(0.0f, 0.5f, 0.0f, 1.0f);
	//float4 v2 = float4(0.5f, 0.0f, 0.0f, 1.0f);
	//float4 v3 = float4(0.5f, 0.5f, 0.0f, 1.0f);
	// Vertice 0
	if (vID == 0)
	{
		vertexPosition = v0;
	}
	// Vertice 2
	else if (vID == 1)
	{
		vertexPosition = v1;
	}
	// Vertice 1
	else if (vID == 2)
	{
		vertexPosition = v2;
	}
	// Vertice 2
	else if (vID == 3)
	{
		vertexPosition = v3;
	}
	// Vertice 1
	else if (vID == 4)
	{
		vertexPosition = v2;
	}
	// Vertice 3
	else if (vID == 5)
	{
		vertexPosition = v1;
	}
	
	//if (vID == 0)
	//{
	//	vertexPosition = v1;
	//}
	//// Vertice 2
	//else if (vID == 1)
	//{
	//	vertexPosition = v3;
	//}
	//// Vertice 1
	//else if (vID == 2)
	//{
	//	vertexPosition = v0;
	//}
	//// Vertice 2
	//else if (vID == 3)
	//{
	//	vertexPosition = float4(0.5f, 0.5f, 0.0f, 1.0f);
	//}
	// Vertice 1
	//else if (vID == 4)
	//{
	//	vertexPosition = float4(0.0f, 0.5f, 0.0f, 1.0f);
	//}
	//// Vertice 3
	//else if (vID == 5)
	//{
	//	vertexPosition = float4(0.5f, 0.5f, 0.0f, 1.0f);
	//}

	output.pos = vertexPosition;
	output.color = cbGUIData.color;
	output.texCoord = float2(v.uv.x, v.uv.y);

	return output;
}