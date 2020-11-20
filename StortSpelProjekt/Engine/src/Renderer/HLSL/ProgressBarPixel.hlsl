#include "../../Headers/structs.h"

struct VS_OUT
{
	float4 pos      : SV_Position;
	float2 uv       : UV;
};

ConstantBuffer<CB_PER_OBJECT_STRUCT> cbPerObject : register(b1, space3);
Texture2D textures[] : register(t0);

// The ProgressBar Specifics is stored in here.
ConstantBuffer<PROGRESS_BAR_DATA> progressBarData : register(b6, space3);

SamplerState point_Wrap : register (s5);

float4 PS_main(VS_OUT input) : SV_TARGET0
{
	// Sample from textures
	float4 texColor	= textures[cbPerObject.info.textureAlbedo].Sample(point_Wrap, input.uv);
	return float4(texColor.rgb, 1.0f);


	// Flat color test
	//// Modified hp bar (red texture) 
	//if (progressBarData.id == 1.0f)
	//{
	//	
	//	return float4(0.8f, 0.0f, 0.0f, 1.0f);
	//}
	//
	//// Background bar (black texture)
	//return float4(0.0f, 0.0f, 0.0f, 1.0f);
}
