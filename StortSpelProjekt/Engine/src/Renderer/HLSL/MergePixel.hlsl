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
	float4 sceneColor = textures[cbPerObject.info.textureMetallic].Sample(point_Wrap, input.uv);
	float4 blurColor = textures[cbPerObject.info.textureAlbedo].Sample(point_Wrap, input.uv);

	// Darken down the base image in areas where there is a lot of bloom
	sceneColor *= (1 - saturate(blurColor));

	// Adjust glowIntensity
	float glowIntensity = 1.5f;
	blurColor = blurColor * glowIntensity;

	// Combine
	float4 finalColor = sceneColor + blurColor;

	// HDR tone mapping
	float4 reinhard = finalColor / (finalColor + float4(1.0f, 1.0f, 1.0f, 1.0f));

	return float4(reinhard.rgb, 1.0f);
}
