#include "../../Headers/structs.h"

struct VS_OUT
{
	float4 pos: SV_POSITION;
	float4 color: COLOR;
	float2 texCoord: TEXCOORD;
};

ConstantBuffer<CB_PER_OBJECT_STRUCT> cbPerObject : register(b1, space3);

Texture2D textures[] : register(t0);
SamplerState point_Wrap : register (s5);

float4 PS_main(VS_OUT input) : SV_TARGET0
{
	//return float4(input.color.rgb, input.color.a * textures[cbPerObject.info.textureAlbedo].Sample(point_Wrap, input.texCoord).a);
	return float4(0.1, 0.1, 0.1, 1.0);
}