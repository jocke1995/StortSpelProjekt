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
	float4 sceneColor = textures[cbPerObject.info.textureMetallic].Sample(samplerTypeWrap, input.uv);
	float4 blurColor = textures[cbPerObject.info.textureAlbedo].Sample(samplerTypeWrap, input.uv);

	// Darken down the base image in areas where there is a lot of bloom
	sceneColor *= (1 - saturate(blurColor));

	// Adjust glowIntensity
	float glowIntensity = 3.0f;
	blurColor = blurColor * glowIntensity;

	// Combine
	float4 finalColor = sceneColor + blurColor;

	// HDR tone mapping
	float exposure = 0.5f;
	finalColor = float4(1.0f, 1.0f, 1.0f, 1.0f) - exp(-finalColor * exposure);

	return finalColor;
}