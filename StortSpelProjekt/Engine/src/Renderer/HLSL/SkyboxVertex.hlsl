#include "../../Headers/structs.h"

struct VS_OUT    //output structure for skymap vertex shader
{
    float4 Pos : SV_POSITION;
    float3 texCoord : TEXCOORD;
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

VS_OUT VS_main(uint vID : SV_VertexID, uint iID : SV_InstanceID)
{
    VS_OUT output = (VS_OUT)0;

    vertex v = meshes[cbPerObject.info.vertexDataIndex][vID];
    float4 vertexPosition = float4(v.pos.xyz, 1.0f);

    //Set Pos to xyww instead of xyzw, so that z will always be 1 (furthest from camera)
    output.Pos = mul(vertexPosition, cbPerObject.WVP).xyww;

    output.texCoord = v.pos;

    return output;
}