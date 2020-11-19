#include "../../Headers/structs.h"

struct VS_OUT
{
	float4 pos      : SV_Position;
	float2 uv       : UV;
};

ConstantBuffer<CB_PER_OBJECT_STRUCT> cbPerObject : register(b1, space3);
Texture2D textures[] : register(t0);

SamplerState point_Wrap : register (s5);

float4 PS_main(VS_OUT input) : SV_TARGET0
{
	// Sample from textures
	//float4 texColor	= textures[cbPerObject.info.textureAlbedo].Sample(point_Wrap, input.uv);
	//return float4(texColor.rgb, texColor.a);

	return float4(0.4f, 0.0f, 0.6f, 1.0f);
}
