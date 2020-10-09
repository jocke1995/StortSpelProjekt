#include "LightCalculations.hlsl"

struct VS_OUT
{
	float4 pos      : SV_Position;
	float4 worldPos : WPos;
	float2 uv       : UV;
	float3x3 tbn	: TBN;
};

ConstantBuffer<DirectionalLight> dirLight[]	: register(b0, space0);
ConstantBuffer<PointLight> pointLight[]		: register(b0, space1);
ConstantBuffer<SpotLight> spotLight[]		: register(b0, space2);

ConstantBuffer<CB_PER_OBJECT_STRUCT> cbPerObject : register(b1, space3);
ConstantBuffer<CB_PER_FRAME_STRUCT>  cbPerFrame  : register(b3, space3);

float4 PS_main(VS_OUT input) : SV_TARGET0
{
	// Sample from textures
	float2 uvScaled = float2(input.uv.x, input.uv.y);
	float4 albedo	= textures[cbPerObject.info.textureAlbedo	].Sample(Anisotropic16_Wrap, uvScaled);
	float roughness = textures[cbPerObject.info.textureRoughness].Sample(Anisotropic16_Wrap, uvScaled).r;
	float metallic	= textures[cbPerObject.info.textureMetallic	].Sample(Anisotropic16_Wrap, uvScaled).r;
	float4 emissive = textures[cbPerObject.info.textureEmissive	].Sample(Anisotropic16_Wrap, uvScaled);
	float4 normal	= textures[cbPerObject.info.textureNormal	].Sample(Anisotropic16_Wrap, uvScaled);

	normal = (2.0f * normal) - 1.0f;
	normal = float4(normalize(mul(normal.xyz, input.tbn)), 1.0f);

	float3 camPos = cbPerFrame.camPos;
	float3 finalColor = float3(0.0f, 0.0f, 0.0f);
	float3 viewDir = normalize(camPos - input.worldPos.xyz);

	// Linear interpolation
	float3 baseReflectivity = lerp(float3(0.04f, 0.04f, 0.04f), albedo.rgb, metallic);

	// DirectionalLight contributions
	for (unsigned int i = 0; i < cbPerScene.Num_Dir_Lights; i++)
	{
		int index = cbPerScene.dirLightIndices[i].x;
		finalColor += CalcDirLight(
			dirLight[index],
			camPos,
			viewDir,
			input.worldPos,
			metallic,
			albedo.rgb,
			roughness,
			normal.rgb,
			baseReflectivity);
	}

	// PointLight contributions
	for (unsigned int i = 0; i < cbPerScene.Num_Point_Lights; i++)
	{
		int index = cbPerScene.pointLightIndices[i].x;

		finalColor += CalcPointLight(
			pointLight[index],
			camPos,
			viewDir,
			input.worldPos,
			metallic,
			albedo.rgb,
			roughness,
			normal.rgb,
			baseReflectivity);
	}

	// SpotLight  contributions
	for (unsigned int i = 0; i < cbPerScene.Num_Spot_Lights; i++)
	{
		int index = cbPerScene.spotLightIndices[i].x;
		finalColor += CalcSpotLight(
			spotLight[index],
			camPos,
			viewDir,
			input.worldPos,
			metallic,
			albedo.rgb,
			roughness,
			normal.rgb,
			baseReflectivity);
	}

	float3 ambient = float3(0.03f, 0.03f, 0.03f) * albedo;
	finalColor += ambient;

	finalColor += emissive.rgb;

	float blendFactor = 0.4f;
	return float4(finalColor.rgb, blendFactor);
}
