#include "../../Headers/structs.h"

struct VS_OUT
{
	float4 pos: SV_POSITION;
	float4 color: COLOR;
	float2 texCoord: TEXCOORD;
};

ConstantBuffer<CB_PER_OBJECT_STRUCT> cbPerObject : register(b1, space3);
ConstantBuffer<CB_PER_GUI_STRUCT> cbGUIData : register(b6, space3);

Texture2D textures[] : register(t0);
SamplerState point_Wrap : register (s5);

float4 PS_main(VS_OUT input) : SV_TARGET0
{
	if (cbGUIData.textureInfo.x == 0)
	{
		return float4(input.color) * cbGUIData.blendFactor;
	}
	else if (cbGUIData.textureInfo.y == 0)
	{
		return float4(input.color * textures[cbPerObject.info.textureAlbedo].Sample(point_Wrap, input.texCoord)) * cbGUIData.blendFactor;
	}
	else
	{
		return float4(input.color * textures[cbPerObject.info.textureEmissive].Sample(point_Wrap, input.texCoord)) * cbGUIData.blendFactor;
	}
}