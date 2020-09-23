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
ConstantBuffer<CB_PER_SCENE_STRUCT>  cbPerScene  : register(b4, space3);

float4 PS_main(VS_OUT input) : SV_TARGET0
{
	// Sample from textures
	float2 uvScaled = float2(input.uv.x * materialAttributes.uvScale.x, input.uv.y * materialAttributes.uvScale.y);
	float4 albedoMap = textures[cbPerObject.info.textureAlbedo		].Sample(samplerTypeWrap, uvScaled);
	float4 roughnessMap = textures[cbPerObject.info.textureRoughness].Sample(samplerTypeWrap, uvScaled);
	float4 metallicMap = textures[cbPerObject.info.textureMetallic	].Sample(samplerTypeWrap, uvScaled);
	float4 emissiveMap = textures[cbPerObject.info.textureEmissive	].Sample(samplerTypeWrap, uvScaled);
	float4 normalMap = textures[cbPerObject.info.textureNormal		].Sample(samplerTypeWrap, uvScaled);

	normalMap = (2.0f * normalMap) - 1.0f;
	float4 normal = float4(normalize(mul(normalMap.xyz, input.tbn)), 1.0f);

	float3 camPos = cbPerFrame.camPos;
	float3 finalColor = float3(0.0f, 0.0f, 0.0f);
	float3 viewDir = normalize(camPos - input.worldPos.xyz);

	// Linear interpolation
	float3 baseReflectivity = lerp(float3(0.04f, 0.04f, 0.04f), albedoMap, metallicMap);

	// DirectionalLight contributions
	for (unsigned int i = 0; i < cbPerScene.Num_Dir_Lights; i++)
	{
		int index = cbPerScene.dirLightIndices[i].x;
		finalColor += CalcDirLight(
			dirLight[index],
			camPos,
			input.worldPos,
			metallicMap.rgb,
			albedoMap.rgb,
			roughnessMap.rgb,
			normal.rgb);
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
			metallicMap.r,
			albedoMap.rgb,
			roughnessMap.r,
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
			input.worldPos,
			metallicMap.rgb,
			albedoMap.rgb,
			roughnessMap.rgb,
			normal.rgb);
	}

	finalColor += emissiveMap.rgb;
	float blendFactor = 0.4f;
	return float4(finalColor.rgb, blendFactor);
}