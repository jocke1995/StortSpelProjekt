#include "../../Headers/structs.h"

struct VS_OUT
{
	float4 pos  : SV_Position;
	float2 uv   : UV;
};

ConstantBuffer<CB_PER_OBJECT_STRUCT> cbPerObject : register(b1, space3);
Texture2D textures[]   : register (t0);

SamplerState point_Wrap	: register (s5);

float4 PS_main(VS_OUT input) : SV_TARGET0
{
	// todo: Use bilnear filtering
	float4 bigTexture = textures[cbPerObject.info.textureMetallic].Sample(point_Wrap, input.uv);

	return bigTexture;
}
