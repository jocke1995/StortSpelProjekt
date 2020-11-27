#include "../../Headers/structs.h"
#include "PBRMath.hlsl"

Texture2D textures[]   : register (t0);

SamplerState Anisotropic2_Wrap				: register (s0);
SamplerState Anisotropic4_Wrap				: register (s1);
SamplerState Anisotropic8_Wrap				: register (s2);
SamplerState Anisotropic16_Wrap				: register (s3);
SamplerState samplerTypeBorder_White		: register (s4);
SamplerState samplerTypeBorder_Black        : register (s6);

ConstantBuffer<CB_PER_SCENE_STRUCT>  cbPerScene  : register(b5, space3);

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

	// Anti aliasing
	float2 texelSize = float2(0.0f, 0.0f);
	textures[shadowMapIndex].GetDimensions(texelSize.x, texelSize.y);
	texelSize = 1.0f / texelSize;

	for (int x = -1; x <= 1; ++x)
	{
		for (int y = -1; y <= 1; ++y)
		{
			float pcfDepth = textures[shadowMapIndex].Sample(samplerTypeBorder_Black, texCoord + float2(x,y) * texelSize).r;
			if (depthFromLightToFragPos > pcfDepth)
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
	in float3 viewDir,
	in float4 fragPos,
	in float metallic,
	in float3 albedo,
	in float roughness,
	in float3 normal,
	in float3 baseReflectivity)
{
	float3 DirLightContribution = float3(0.0f, 0.0f, 0.0f);

	float3 lightDir = normalize(-dirLight.direction.rgb);
	float3 normalized_bisector = normalize(viewDir + lightDir);

	float3 radiance = dirLight.baseLight.color.rgb;

	// Cook-Torrance BRDF
	float NdotV = max(dot(normal, viewDir), 0.0000001);
	float NdotL = max(dot(normal, lightDir), 0.0000001);
	float HdotV = dot(normalized_bisector, viewDir);
	float HdotN = dot(normalized_bisector, normal);

	float  D = NormalDistributionGGX(HdotN, roughness);
	float  G = GeometrySmith(NdotV, NdotL, roughness);
	float3 F = CalculateFresnelEffect(HdotV, baseReflectivity);

	float3 specular = D * G * F / (4.0f * NdotV * NdotL);

	// Energy conservation
	float3 kD = float3(1.0f, 1.0f, 1.0f) - F;
	kD *= 1.0f - metallic;

	// Shadows
	float shadow = 0.0f;
	if (dirLight.baseLight.castShadow == true)
	{
		float4 fragPosLightSpace = mul(fragPos, dirLight.viewProj);

		shadow = CalculateShadow(fragPosLightSpace, dirLight.textureShadowMap);
	}

	DirLightContribution = (kD * albedo / PI + specular) * radiance * NdotL;
	return DirLightContribution * (1.0f - shadow);
}

float3 CalcPointLight(
	in PointLight pointLight,
	in float3 camPos,
	in float3 viewDir,
	in float4 fragPos,
	in float metallic,
	in float3 albedo,
	in float roughness,
	in float3 normal,
	in float3 baseReflectivity)
{
	float3 pointLightContribution = float3(0.0f, 0.0f, 0.0f);

	float3 lightDir = normalize(pointLight.position - fragPos.xyz);
	float3 normalized_bisector = normalize(viewDir + lightDir);

	// Attenuation
	float constantFactor = pointLight.attenuation.x;
	float linearFactor = pointLight.attenuation.y;
	float quadraticFactor = pointLight.attenuation.z;
	float distancePixelToLight = length(pointLight.position.xyz - fragPos);
	float attenuation = 1.0f / (constantFactor + (linearFactor * distancePixelToLight) + (quadraticFactor * pow(distancePixelToLight, 2)));

	float3 radiance = pointLight.baseLight.color.rgb * attenuation; 

	// Cook-Torrance BRDF
	float NdotV = max(dot(normal, viewDir), 0.0000001);
	float NdotL = max(dot(normal, lightDir), 0.0000001);
	float HdotV = dot(normalized_bisector, viewDir);
	float HdotN = dot(normalized_bisector, normal);

	float  D = NormalDistributionGGX(HdotN, roughness);
	float  G = GeometrySmith(NdotV, NdotL, roughness);
	float3 F = CalculateFresnelEffect(HdotV, baseReflectivity);

	float3 specular = D * G * F / (4.0f * NdotV * NdotL);

	// Energy conservation
	float3 kD = float3(1.0f, 1.0f, 1.0f) - F;
	kD *= 1.0f - metallic;

	pointLightContribution = (kD * albedo / PI + specular) * radiance * NdotL;
	return pointLightContribution;
}

float3 CalcSpotLight(
	in SpotLight spotLight,
	in float3 camPos,
	in float3 viewDir,
	in float4 fragPos,
	in float metallic,
	in float3 albedo,
	in float roughness,
	in float3 normal,
	in float3 baseReflectivity)
{
	float3 spotLightContribution = float3(0.0f, 0.0f, 0.0f);
	
	float3 lightDir = normalize(spotLight.position_cutOff.xyz - fragPos.xyz);
	float3 normalized_bisector = normalize(viewDir + lightDir);
	
	// Calculate the angle between lightdir and the direction of the light
	float theta = dot(lightDir, normalize(-spotLight.direction_outerCutoff.xyz));

	// To smooth edges
	float epsilon = (spotLight.position_cutOff.w - spotLight.direction_outerCutoff.w);
	float edgeIntensity = clamp((theta - spotLight.direction_outerCutoff.w) / epsilon, 0.0f, 1.0f);

	// Attenuation
	float constantFactor = spotLight.attenuation.x;
	float linearFactor = spotLight.attenuation.y;
	float quadraticFactor = spotLight.attenuation.z;
	float distancePixelToLight = length(spotLight.position_cutOff.xyz - fragPos);
	float attenuation = 1.0f / (constantFactor + (linearFactor * distancePixelToLight) + (quadraticFactor * pow(distancePixelToLight, 2)));

	float3 radiance = spotLight.baseLight.color.rgb * attenuation;

	// Cook-Torrance BRDF
	float NdotV = max(dot(normal, viewDir), 0.0000001);
	float NdotL = max(dot(normal, lightDir), 0.0000001);
	float HdotV = dot(normalized_bisector, viewDir);
	float HdotN = dot(normalized_bisector, normal);

	float  D = NormalDistributionGGX(HdotN, roughness);
	float  G = GeometrySmith(NdotV, NdotL, roughness);
	float3 F = CalculateFresnelEffect(HdotV, baseReflectivity);

	float3 specular = D * G * F / (4.0f * NdotV * NdotL);

	// Energy conservation
	float3 kD = float3(1.0f, 1.0f, 1.0f) - F;
	kD *= 1.0f - metallic;

	float shadow = 0.0f;
	if (spotLight.baseLight.castShadow == true)
	{
		float4 fragPosLightSpace = mul(fragPos, spotLight.viewProj);

		shadow = CalculateShadow(fragPosLightSpace, spotLight.textureShadowMap);
	}

	spotLightContribution = ((kD * albedo / PI + specular) * radiance * NdotL) * edgeIntensity;
	spotLightContribution = spotLightContribution * (1.0f - shadow);
	return spotLightContribution;
}
