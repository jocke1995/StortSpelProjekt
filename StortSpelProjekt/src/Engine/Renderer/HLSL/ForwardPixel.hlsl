#include "LightCalculations.hlsl"

struct VS_OUT
{
	float4 pos      : SV_Position;
	float4 worldPos : WPos;
	float4 uv       : UV;
	float3x3 tbn	: TBN;
};

ConstantBuffer<DirectionalLight> dirLight[]	: register(b0, space0);
ConstantBuffer<PointLight> pointLight[]		: register(b0, space1);
ConstantBuffer<SpotLight> spotLight[]		: register(b0, space2);

ConstantBuffer<CB_PER_OBJECT_STRUCT> cbPerObject : register(b1, space3);
ConstantBuffer<CB_PER_FRAME_STRUCT>  cbPerFrame  : register(b2, space3);
ConstantBuffer<CB_PER_SCENE_STRUCT>  cbPerScene  : register(b3, space3);

float4 PS_main(VS_OUT input) : SV_TARGET0
{
	float3 camPos = cbPerFrame.camPos;
	float3 finalColor = float3(0.0f, 0.0f, 0.0f);

	// Sample from textures
	float4 ambientMap  = textures[cbPerObject.info.textureAmbient ].Sample(samplerTypeWrap, input.uv);
	float4 diffuseMap  = textures[cbPerObject.info.textureDiffuse ].Sample(samplerTypeWrap, input.uv);
	float4 specularMap = textures[cbPerObject.info.textureSpecular].Sample(samplerTypeWrap, input.uv);
	float4 emissiveMap = textures[cbPerObject.info.textureEmissive].Sample(samplerTypeWrap, input.uv);
	float4 normalMap   = textures[cbPerObject.info.textureNormal  ].Sample(samplerTypeWrap, input.uv);

	normalMap = (2.0f * normalMap) - 1.0f;
	float4 normal = float4(normalize(mul(normalMap.xyz, input.tbn)), 1.0f);

	// DirectionalLight contributions
	for (unsigned int i = 0; i < cbPerScene.Num_Dir_Lights; i++)
	{
		int index = cbPerScene.dirLightIndices[i].x;
		finalColor += CalcDirLight(
			dirLight[index],
			camPos,
			input.worldPos,
			ambientMap.rgb,
			diffuseMap.rgb,
			specularMap.rgb,
			normal.rgb);
	}

	// PointLight contributions
	for (unsigned int i = 0; i < cbPerScene.Num_Point_Lights; i++)
	{
		int index = cbPerScene.pointLightIndices[i].x;
		finalColor += CalcPointLight(
			pointLight[index],
			camPos,
			input.worldPos,
			ambientMap.rgb,
			diffuseMap.rgb,
			specularMap.rgb,
			normal.rgb);
	}

	// SpotLight  contributions
	for (unsigned int i = 0; i < cbPerScene.Num_Spot_Lights; i++)
	{
		int index = cbPerScene.spotLightIndices[i].x;
		finalColor += CalcSpotLight(
			spotLight[index],
			camPos,
			input.worldPos,
			ambientMap.rgb,
			diffuseMap.rgb,
			specularMap.rgb,
			normal.rgb);
	}

	finalColor += emissiveMap.rgb;
	finalColor = saturate(finalColor);
	return float4(finalColor.rgb, 1.0f);
}