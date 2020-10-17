#include "../../Headers/structs.h"

struct VS_OUT
{
	float4 pos: SV_POSITION;
	float4 color: COLOR;
	float2 texCoord: TEXCOORD;
};

struct vertex
{
	float4 pos;
	float4 texCoord;
	float4 color;
};

ConstantBuffer<CB_PER_GUI2D_OBJECT_STRUCT> cbPerObject : register(b1, space3);

StructuredBuffer<vertex> meshes[] : register(t0);

VS_OUT VS_main(uint vID : SV_VertexID, uint iID : SV_InstanceID)
{
	VS_OUT output;

	// vert id 0 = 0000, uv = (0, 0)
	// vert id 1 = 0001, uv = (1, 0)
	// vert id 2 = 0010, uv = (0, 1)
	// vert id 3 = 0011, uv = (1, 1)
	float2 uv = float2(vID & 1, (vID >> 1) & 1);

	// set the position for the vertex based on which vertex it is (uv)
	vertex mesh = meshes[cbPerObject.info.vertexDataIndex][iID];
	output.pos = float4(mesh.pos.x + (mesh.pos.z * uv.x), mesh.pos.y - (mesh.pos.w * uv.y), 0, 1);
	output.color = mesh.color;

	// set the texture coordinate based on which vertex it is (uv)
	output.texCoord = float2(mesh.texCoord.x + (mesh.texCoord.z * uv.x), mesh.texCoord.y + (mesh.texCoord.w * uv.y));

	return output;
}