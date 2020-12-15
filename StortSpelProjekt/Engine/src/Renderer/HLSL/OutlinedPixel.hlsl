struct VS_OUT
{
	float4 pos      : SV_Position;
};

float4 PS_main(VS_OUT input) : SV_TARGET0
{
	// Purple
	return float4(75/255.0f, 0.0f, 130/255.0f, 1.0f);

	// Red
	//return float4(1.0f, 0.0f, 0.0f, 1.0f);

	// Green
	//return float4(0.0f, 1.0f, 0.0f, 1.0f);

	// Blue
	//return float4(0.0f, 0.0f, 1.0f, 1.0f);
}