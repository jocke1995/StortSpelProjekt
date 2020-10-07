#include "../../Headers/structs.h"

struct VS_OUT    //output structure for skymap vertex shader
{
    float4 Pos : SV_POSITION;
    float3 texCoord : TEXCOORD;
};

ConstantBuffer<CB_PER_OBJECT_STRUCT> cbPerObject : register(b1, space3);
TextureCube textures[] : register(t0);

SamplerState point_Wrap : register (s5);

float4 PS_main(VS_OUT input) : SV_Target
{
    return textures[cbPerObject.info.textureAlbedo].Sample(point_Wrap, input.texCoord);
}