// Source: https://www.youtube.com/watch?v=5p0e7YNONr8&ab_channel=BrianWill

static const float PI = 3.141572;

// Proportion of specular reflectance
float3 CalculateFresnelEffect(float HdotV, float3 baseReflectivity)
{
	float3 IdentityVector = float3(1.0f, 1.0f, 1.0f);

	return baseReflectivity + (IdentityVector - baseReflectivity) * pow(IdentityVector - HdotV, 5.0f);
}

// Approximate the specular reflectivity based on the roughness of the material
// In this case, we use Trowbridge-Reitz GGX normal distribution function
float NormalDistributionGGX(float NdotH, float roughness)
{
	float r2 = roughness * roughness;
	float NdotH2 = NdotH * NdotH;

	float denominator = PI * pow((NdotH2 * (r2 - 1) + 1), 2);
	
	// Prevent division by zero
	denominator = max(denominator, 0.00000001f);

	return r2 / denominator;
}

// Approximate the self-shadowing for each microsurface
float GeometrySmith(float NdotV, float NdotL, float roughness)
{
	float k = pow(roughness + 1, 2) / 8;

	float ggx1 = NdotV / (NdotV * (1 - k) + k);
	float ggx2 = NdotL / (NdotL * (1 - k) + k);

	return ggx1 * ggx2;
}