#include "../../Headers/structs.h"

struct VS_OUT
{
	float4 pos: SV_POSITION;
	float4 color: COLOR;
	float2 texCoord: TEXCOORD;
};

struct textVertex
{
	float4 pos;
	float4 texCoord;
	float4 color;
};

ConstantBuffer<CB_PER_OBJECT_STRUCT> cbPerObject : register(b1, space3);

StructuredBuffer<textVertex> texts[] : register(t0);

VS_OUT VS_main(uint vID : SV_VertexID, uint iID : SV_InstanceID)
{
	VS_OUT output = (VS_OUT)0;

	// vert id 0 = 0000, uv = (0, 0)
	// vert id 1 = 0001, uv = (1, 0)
	// vert id 2 = 0010, uv = (0, 1)
	// vert id 3 = 0011, uv = (1, 1)
	float2 uv = float2(vID & 1, (vID >> 1) & 1);

	// set the position for the vertex based on which vertex it is (uv)
	textVertex text = texts[cbPerObject.info.vertexDataIndex][iID];
	output.pos = float4(text.pos.x + (text.pos.z * uv.x), text.pos.y - (text.pos.w * uv.y), 0, 1);
	output.color = text.color;

	// set the texture coordinate based on which vertex it is (uv)
	output.texCoord = float2(text.texCoord.x + (text.texCoord.z * uv.x), text.texCoord.y + (text.texCoord.w * uv.y));

	return output;
}