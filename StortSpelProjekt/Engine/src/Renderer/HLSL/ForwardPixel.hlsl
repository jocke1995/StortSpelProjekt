#include "LightCalculations.hlsl"

struct VS_OUT
{
	float4 pos      : SV_Position;
	float4 worldPos : WPos;
	float2 uv       : UV;
	float3x3 tbn	: TBN;
};

struct PS_OUTPUT
{
	float4 sceneColor: SV_TARGET0;
	float4 brightColor: SV_TARGET1;
};

ConstantBuffer<DirectionalLight> dirLight[]	: register(b0, space0);
ConstantBuffer<PointLight> pointLight[]		: register(b0, space1);
ConstantBuffer<SpotLight> spotLight[]		: register(b0, space2);

ConstantBuffer<CB_PER_OBJECT_STRUCT> cbPerObject : register(b1, space3);
ConstantBuffer<CB_PER_FRAME_STRUCT>  cbPerFrame  : register(b3, space3);
ConstantBuffer<CB_PER_SCENE_STRUCT>  cbPerScene  : register(b4, space3);

PS_OUTPUT PS_main(VS_OUT input)
{
	float3 camPos = cbPerFrame.camPos;
	float3 finalColor = float3(0.0f, 0.0f, 0.0f);

	// Sample from textures
	float2 uvScaled = float2(input.uv.x * materialAttributes.uvScale.x, input.uv.y * materialAttributes.uvScale.y);
	float4 ambientMap  = textures[cbPerObject.info.textureAmbient ].Sample(samplerTypeWrap, uvScaled);
	float4 diffuseMap  = textures[cbPerObject.info.textureDiffuse ].Sample(samplerTypeWrap, uvScaled);
	float4 specularMap = textures[cbPerObject.info.textureSpecular].Sample(samplerTypeWrap, uvScaled);
	float4 emissiveMap = textures[cbPerObject.info.textureEmissive].Sample(samplerTypeWrap, uvScaled);
	float4 normalMap   = textures[cbPerObject.info.textureNormal  ].Sample(samplerTypeWrap, uvScaled);

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

	PS_OUTPUT output;
	output.sceneColor = float4(finalColor.rgb, 1.0f);

	float brightness = dot(output.sceneColor.rgb, float3(0.2126, 0.7152, 0.0722));
	if (brightness > 0.4)
	{
		output.brightColor = output.sceneColor;
	}
	else
	{
		output.brightColor = float4(0.0, 0.0, 0.0, 1.0);
	}

	// For debugging the brightColor
	// output.sceneColor = output.brightColor;
	return output;
}