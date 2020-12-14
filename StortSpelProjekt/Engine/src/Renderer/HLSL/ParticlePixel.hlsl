#include "../../Headers/structs.h"

struct VS_OUT
{
	float4 pos      : SV_Position;
	float2 uv       : UV;
	uint iID		: ID;
};

ConstantBuffer<CB_PER_OBJECT_STRUCT> cbPerObject : register(b1, space3);
Texture2D textures[] : register(t0);

SamplerState point_Wrap : register (s5);

StructuredBuffer<PARTICLE_DATA> particleData : register(t3, space3); // Edited by billboard compute

float4 PS_main(VS_OUT input) : SV_TARGET0
{
	// Sample from textures
	float4 texColor	= textures[cbPerObject.info.textureAlbedo].Sample(point_Wrap, input.uv);

	float4 pixelColor = texColor * particleData[input.iID].color;

	return pixelColor;
}
