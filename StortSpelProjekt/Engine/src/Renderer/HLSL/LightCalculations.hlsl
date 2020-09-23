#include "../../Headers/structs.h"

Texture2D textures[]   : register (t0);
SamplerState samplerTypeWrap	: register (s0);
SamplerState samplerTypeBorder	: register (s1);

static const MaterialAttributes materialAttributes = {
	100,
	0, 0, 0,
	
	0, 0, 0, 0,
	0, 0, 0, 0,
	0, 0, 0, 0,

	1, 1, 1, 1,
	1, 1, 1, 1,
	1, 1, 1, 1,

	1, 1,
	0, 0
};

float CalculateShadow(
	in float4 fragPosLightSpace,
	in float shadowMapIndex)
{
	// Perform perspective divide
	float2 texCoord = fragPosLightSpace.xy / fragPosLightSpace.w;

	// Transform to [0,1] range
	texCoord = texCoord * 0.5 + 0.5;
	texCoord.y = 1 - texCoord.y;

	// Get depth of current fragment from light's perspective
	float depthFromLightToFragPos = fragPosLightSpace.z / fragPosLightSpace.w;

	// Check whether current fragPos is in shadow
	float shadow = 0.0f;
	float bias = 0.0003f;

	// Anti aliasing
	float2 texelSize = float2(0.0f, 0.0f);
	textures[shadowMapIndex].GetDimensions(texelSize.x, texelSize.y);
	texelSize = 1.0f / texelSize;

	for (int x = -1; x <= 1; ++x)
	{
		for (int y = -1; y <= 1; ++y)
		{
			float pcfDepth = textures[shadowMapIndex].Sample(samplerTypeBorder, texCoord + float2(x,y) * texelSize).r;
			if (depthFromLightToFragPos - bias > pcfDepth)
			{
				shadow += 1.0f;
			}
		}
	}
	shadow = shadow / 9.0f;

	return shadow;
}

float3 CalcDirLight(
	in DirectionalLight dirLight,
	in float3 camPos,
	in float4 fragPos,
	in float3 ambientMap,
	in float3 diffuseMap,
	in float3 specularMap,
	in float3 normalMap)
{
	// Ambient
	float3 ambient = ambientMap * dirLight.baseLight.intensity.rgb * materialAttributes.ambientMul + materialAttributes.ambientAdd;

	// Diffuse
	float3 lightDir = normalize(-dirLight.direction.xyz);
	float alpha = max(dot(normalMap, lightDir), 0.0f);
	float3 diffuse = diffuseMap * alpha * dirLight.baseLight.intensity.rgb * materialAttributes.diffuseMul + materialAttributes.diffuseAdd;

	// Specular
	float3 vecToCam = normalize(camPos - fragPos.xyz);
	float3 reflection = normalize(reflect(-lightDir.xyz, normalMap.xyz));
	float spec = pow(max(dot(reflection, vecToCam), 0.0), materialAttributes.shininess);
	float3 specular = (specularMap.rgb * dirLight.baseLight.intensity.rgb * materialAttributes.specularMul + materialAttributes.specularAdd) * spec;

	float shadow = 0.0f;
	if (dirLight.baseLight.castShadow == true)
	{
		float4 fragPosLightSpace = mul(fragPos, dirLight.viewProj);

		shadow = CalculateShadow(fragPosLightSpace, dirLight.textureShadowMap);
	}

	float3 DirLightContribution = ambient.rgb + ((1.0f - shadow) * (diffuse.rgb + specular.rgb));
	return DirLightContribution;
}

float3 CalcPointLight(
	in PointLight pointLight,
	in float3 camPos,
	in float4 fragPos,
	in float3 ambientMap,
	in float3 diffuseMap,
	in float3 specularMap,
	in float3 normalMap)
{
	float3 pointLightContribution = float3(0.0f, 0.0f, 0.0f);

	// Ambient
	float3 ambient = ambientMap * pointLight.baseLight.intensity.rgb * materialAttributes.ambientMul + materialAttributes.ambientAdd;

	// Diffuse
	float3 lightDir = normalize(pointLight.position.xyz - fragPos.xyz);
	float alpha = max(dot(normalMap, lightDir), 0.0f);
	float3 diffuse = diffuseMap * alpha * pointLight.baseLight.intensity.rgb * materialAttributes.diffuseMul + materialAttributes.diffuseAdd;

	// Specular
	float3 vecToCam = normalize(camPos - fragPos.xyz);
	float3 reflection = normalize(reflect(-lightDir.xyz, normalMap.xyz));
	float spec = pow(max(dot(reflection, vecToCam), 0.0), 100);
	float3 specular = (specularMap.rgb * pointLight.baseLight.intensity.rgb * materialAttributes.specularMul + materialAttributes.specularAdd) * spec;

	// Attenuation
	float constantFactor = pointLight.attenuation.x;
	float linearFactor = pointLight.attenuation.y;
	float quadraticFactor = pointLight.attenuation.z;

	float distancePixelToLight = length(pointLight.position.xyz - fragPos.xyz);
	float attenuation = 1.0f / (constantFactor + (linearFactor * distancePixelToLight) + (quadraticFactor * pow(distancePixelToLight, 2)));

	pointLightContribution = float3(ambient.rgb + attenuation * (diffuse.rgb + specular.rgb));

	return pointLightContribution;
}

float3 CalcSpotLight(
	in SpotLight spotLight,
	in float3 camPos,
	in float4 fragPos,
	in float3 ambientMap,
	in float3 diffuseMap,
	in float3 specularMap,
	in float3 normalMap)
{
	float3 lightDir = normalize(spotLight.position_cutOff.xyz - fragPos);
	
	// Calculate the angle between lightdir and the direction of the light
	float theta = dot(lightDir, normalize(-spotLight.direction_outerCutoff.xyz));

	// To smooth edges
	float epsilon = (spotLight.position_cutOff.w - spotLight.direction_outerCutoff.w);
	float intensity = clamp((theta - spotLight.direction_outerCutoff.w) / epsilon, 0.0f, 1.0f);

	float3 spotLightContribution = float3(0.0f, 0.0f, 0.0f);

	// Ambient
	float3 ambient = ambientMap * spotLight.baseLight.intensity.rgb * materialAttributes.ambientMul + materialAttributes.ambientAdd;;

	// Diffuse
	float alpha = max(dot(normalMap, lightDir), 0.0f);
	float3 diffuse = diffuseMap * alpha * spotLight.baseLight.intensity.rgb * materialAttributes.diffuseMul + materialAttributes.diffuseAdd;;

	// Specular
	float3 vecToCam = normalize(camPos - fragPos);
	float3 reflection = normalize(reflect(-lightDir.xyz, normalMap.xyz));
	float spec = pow(max(dot(reflection, vecToCam), 0.0), 100);
	float3 specular = (specularMap.rgb * spotLight.baseLight.intensity.rgb * materialAttributes.specularMul + materialAttributes.specularAdd) * spec;

	// Attenuation
	float constantFactor = spotLight.attenuation.x;
	float linearFactor = spotLight.attenuation.y;
	float quadraticFactor = spotLight.attenuation.z;

	float distancePixelToLight = length(spotLight.position_cutOff.xyz - fragPos);
	float attenuation = 1.0f / (constantFactor + (linearFactor * distancePixelToLight) + (quadraticFactor * pow(distancePixelToLight, 2)));
		
	float shadow = 0.0f;
	if (spotLight.baseLight.castShadow == true)
	{
		float4 fragPosLightSpace = mul(fragPos, spotLight.viewProj);

		shadow = CalculateShadow(fragPosLightSpace, spotLight.textureShadowMap);
	}

	spotLightContribution =  float3(ambient.rgb + (1.0f - shadow) * attenuation* (diffuse.rgb + specular.rgb)) * intensity;
	
	return spotLightContribution;
}
