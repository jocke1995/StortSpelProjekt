#include "../../Headers/structs.h"

struct VS_OUT
{
	float4 pos  : SV_Position;
	float2 uv   : UV;
};

// Source & Destination descriptorHeapIndices are stored in metallic & albedo
ConstantBuffer<CB_PER_OBJECT_STRUCT> descriptorHeapIndices : register(b1, space3);

Texture2D<float4> textures[]   : register (t0);

SamplerState linear_Wrap	: register (s5);

float4 PS_main(VS_OUT input) : SV_TARGET0
{
	float4 outputFiltered = textures[descriptorHeapIndices.info.textureAlbedo].Sample(linear_Wrap, input.uv);

	return outputFiltered;
}
