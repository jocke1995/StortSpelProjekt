#include "../../Headers/structs.h"

struct VS_OUT
{
	float4 pos  : SV_Position;
	float2 uv   : UV;
};

ConstantBuffer<CB_PER_OBJECT_STRUCT> cbPerObject : register(b1, space3);
Texture2D textures[]   : register (t0);

SamplerState samplerTypeWrap	: register (s0);

float4 PS_main(VS_OUT input) : SV_TARGET0
{
	float4 sceneColor = textures[cbPerObject.info.textureAmbient].Sample(samplerTypeWrap, input.uv);
	float4 blurColor = textures[cbPerObject.info.textureDiffuse].Sample(samplerTypeWrap, input.uv);

	// Darken down the base iage in areas where there is a lot of bloom
	sceneColor *= (1 - saturate(blurColor));

	// Combine
	float glowIntensity = 1.0f;
	return sceneColor + (blurColor * glowIntensity);

	// for debugging
	// return blurColor;
}