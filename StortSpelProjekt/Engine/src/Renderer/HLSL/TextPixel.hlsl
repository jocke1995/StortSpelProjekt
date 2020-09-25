#include "../../Headers/structs.h"
Texture2D textures[] : register(t0);
SamplerState samplerTypeWrap : register (s0);

struct VS_OUT
{
	float4 pos: SV_POSITION;
	float4 color: COLOR;
	float2 texCoord: TEXCOORD;
};

ConstantBuffer<CB_PER_OBJECT_STRUCT> cbPerObject : register(b1, space3);

float4 PS_main(VS_OUT input) : SV_TARGET0
{
	return float4(input.color.rgb, input.color.a * textures[cbPerObject.info.textureAlbedo].Sample(samplerTypeWrap, input.texCoord).a);
}