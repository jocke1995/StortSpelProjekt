#include "../../Headers/structs.h"

struct VS_OUT    //output structure for skymap vertex shader
{
    float4 Pos : SV_POSITION;
    float3 texCoord : TEXCOORD;
};

ConstantBuffer<CB_PER_OBJECT_STRUCT> cbPerObject : register(b1, space3);
TextureCube textures[] : register(t0);

SamplerState cubeSampler : register (s2);

float4 PS_main(VS_OUT input) : SV_Target
{
    return 1*textures[cbPerObject.info.textureAlbedo].Sample(cubeSampler, input.texCoord);
}